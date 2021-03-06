import common_pb2
import descriptor_pb2
import argparse
import os
from collections import defaultdict
from jinja2 import Environment, PackageLoader, Template

VERBOSE = False

EXCLUDE_MESSAGES = set([
    'Color4',
#    'Settings',
    'Matrix3x3',
    'Matrix4x4',
    ])

EDITOR_NATIVE_MESSAGE_TYPES = {
    'Vector2'   : 'Vector2f',
    'Vector3'   : 'Vector3f',
    'Vector4'   : 'Vector4f', 
    'Color4'    : 'Color', 
    'Matrix4x4' : 'Transform', 
}

ENGINE_NATIVE_MESSAGE_TYPES = {
    'Vector2'   : 'Vector2',
    'Vector3'   : 'Vector3',
    'Vector4'   : 'Vector4', 
    'Color4'    : 'Color', 
    'Matrix4x4' : 'Matrix', 
}

EXCLUDE_FORWARD_DECL = set([
    'Vector2',
    'Vector3',
    'Vector4',
    ])

FORWARD_DECL = {
#    'Vector3f FromProtocol(const common::Vector3& p);',
}

NATIVE_TYPES = {
    1   : 'double',
    2   : 'float',
    3   : 's64',
    4   : 'u64',
    5   : 's32',
    6   : 's64',
    7   : 's32',
    8   : 'bool',
    9   : 'string',
    13  : 'u32',
    15  : 's32',
    16  : 's64',
    17  : 's32',
    18  : 's64',
}

def underscore_to_camel_case(str):
    # 'under_score_hej' -> 'underScoreHej'
    x = str.split('_')
    return x[0] + ''.join(map(lambda x : x.title(), x[1:]))

def underscore_to_title(str):
    # 'under_score_hej' -> 'UnderScoreHej'
    x = str.split('_')
    return ''.join(map(lambda x : x.title(), x[:]))

def underscore_to_title_case(str):
    # 'under_score_hej' -> 'underScoreHej'
    x = str.split('_')
    return ''.join(map(lambda x : x.title(), x))

def package_to_filename(package):
    return '_'.join(package.split('.')[1:])

def is_package_blacklisted(package):
    package_split = package.split('.')
    blacklist = ['anttweak', 'protobuf']
    return any([x in package_split for x in blacklist])
    
def parse_descriptor_set(cmdargs, NATIVE_MESSAGE_TYPES):
    try: 
        os.makedirs(cmdargs.outdir); 
    except: 
        pass

    # todo: this is causing everything to be run twice!
    env = Environment(
        loader=PackageLoader('codegen', 'templates'),
        trim_blocks = True,
        lstrip_blocks = True)

    fds = descriptor_pb2.FileDescriptorSet()
    fds.ParseFromString(open('effects.desc', 'rb').read())

    file_to_package = {}
    # group the files by package
    desc_by_package = defaultdict(list)
    for file_desc in fds.file:
        desc_by_package[file_desc.package].append(file_desc)
        file_to_package[file_desc.name] = file_desc.package

    for package, file_descs in desc_by_package.iteritems():

        if is_package_blacklisted(package):
            continue

        package_split = package.split('.')
        package_namespace = '::'.join(package_split)
        out_name = package_to_filename(package)
        
        # the 'protocol' tag shouldn't be a part of the namespace
        ns = package.split('.')[1:]
        
        args = { 
            'classes' : [], 
            'all_classes' : [], 
            'dependencies' : [],
            'cpp_files' : [],
            'hpp_files' : [],
            'out_name' : out_name,
            'namespace' : cmdargs.namespace,
            'namespace_open' : 'namespace ' + cmdargs.namespace + ' { ' + ''.join(['namespace ' + x + ' { ' for x in ns]),
            'namespace_close' : '} ' * (1 + len(ns))
         }

        # if the package namespace is the same as the command line namespace, don't duplicate
        if len(ns) == 1 and ns[0] == cmdargs.namespace:
            args['namespace_open'] = 'namespace ' + cmdargs.namespace + ' { '
            args['namespace_close'] = '}'

        # iterate over FileDescriptor in the current package
        for file_desc in file_descs:
        
            # add the dependencies (imports)
            dep_set = set()
            for dep in file_desc.dependency:
                if not is_package_blacklisted(dep):
                    pkg = file_to_package[dep]
                    dep_set.add(package_to_filename(pkg))
            args['dependencies'] = set(args['dependencies']).union(dep_set)

            args['cpp_files'].append(file_desc.name.replace('.proto', '.pb.cc'))
            args['hpp_files'].append(file_desc.name.replace('.proto', '.pb.h'))

            flag_vars = set()

            # iterator over Descriptor (messages)
            for msg_desc in file_desc.message_type:

                if msg_desc.name in EXCLUDE_MESSAGES:
                    continue

                if msg_desc.name not in EXCLUDE_FORWARD_DECL:
                    args['all_classes'].append(msg_desc.name)
                args['forward_decls'] = FORWARD_DECL

                # don't create structs for message types we have native implementations for
                if msg_desc.name in NATIVE_MESSAGE_TYPES:
                    continue

                cur_class = { 
                    'members' : [], 
                    'enums' : [],
                    'flags' : [],
                    'proto_type' : '::' + package_namespace + '::' + msg_desc.name,
                    }

                cur_class['name'] = msg_desc.name
                args['classes'].append(cur_class)

                # extract any enums
                for enum in msg_desc.enum_type:
                    # if the enum is named 'Flags', treat it as a flag bitfield
                    flag = enum.name.split('Flags')
                    if len(flag) == 2:
                        f = { 'name' : flag[0], 'vals' : []}
                        cur_class['flags'].append(f)
                        flag_vars.add(flag[0].lower() + '_flags')
                        for i, value in enumerate(enum.value):
                            f['vals'].append({'name' : value.name, 'value' : i})
                    else:
                        # normal enum
                        e = { 'name' : enum.name, 'vals' : []}
                        cur_class['enums'].append(e)
                        for value in enum.value:
                            e['vals'].append({'name' : value.name, 'number' : value.number})

                # process fields
                idx = 0
                for field_desc in msg_desc.field:
                    type_number = field_desc.type

                    is_msg = type_number == 11
                    is_bytes = type_number == 12
                    is_enum = type_number == 14
                    is_native = type_number in NATIVE_TYPES
                    is_optional = field_desc.label == 1
                    is_required = field_desc.label == 2
                    is_repeated = field_desc.label == 3
                    is_flag = False

                    proto_type = field_desc.type_name

                    proto_field_name = field_desc.name
                    field_name = underscore_to_camel_case(field_desc.name)
                    name_title = underscore_to_title_case(field_desc.name)
                    s = field_desc.type_name.split('.')[1:]

                    # if the package is of the form 'protocol::namespace::type', the c++ namespace can
                    # be omitted
                    if len(s) == 3 and s[1] == cmdargs.namespace:
                        field_namespace = ''
                    else:
                        field_namespace = '::' + cmdargs.namespace + '::' + '::'.join(s[1:-1]) + '::'

                    default_value = None
                    if len(field_desc.default_value) > 0:
                        # handle special cases for default value
                        if is_enum:
                            default_value = field_desc.type_name.split('.')[-1] + '::' + field_desc.default_value
                        else:
                            # add trailing 'f' to float values to avoid compiler warnings
                            if field_type == 'float' and '.' in field_desc.default_value:
                                default_value = field_desc.default_value + 'f'
                            else:
                                default_value = field_desc.default_value

                    if type_number in NATIVE_TYPES:
                        # check if the var is a flag, instead of a normal type
                        if field_desc.name in flag_vars:
                            field_type = underscore_to_title(field_desc.name)
                            base_type = field_type
                            default_value = None
                            is_flag = True
                        else:
                            field_type = NATIVE_TYPES[type_number]
                            base_type = field_type
                    elif is_msg:
                        proto_type = ''.join(s[1:])
                        suffix = field_desc.type_name.split('.')[-1]
                        field_type = NATIVE_MESSAGE_TYPES.get(suffix, suffix)
                        
                        # use the native message type if one exists
                        if suffix in NATIVE_MESSAGE_TYPES or package_split == s[:-1]:
                            field_type = NATIVE_MESSAGE_TYPES.get(suffix, suffix)
                        else:
                            field_type = '::' + cmdargs.namespace + '::' + '::'.join(s[1:-1]) + '::' + NATIVE_MESSAGE_TYPES.get(suffix, suffix)
                        base_type = field_type
                    elif is_enum:
                        s = field_desc.type_name.split('.')
                        # convert .effect.plexus.NoiseEffector.ApplyTo to NoiseEffector::ApplyTo
                        outer = field_desc.type_name.split('.')[-2]
                        field_type = outer + '::' + field_desc.type_name.split('.')[-1]
                        base_type = field_type
                    elif is_bytes:
                        field_type = 'vector<uint8_t>'
                    else:
                        print '** UNHANDLED TYPE: ', type_number

                    # if repeated, convert to 'vector<type_name>'
                    if is_repeated:
                        base_type = field_type
                        field_type = 'vector<%s>' % field_type

                    cur_member = { 
                        'name' : field_name,
                        'name_title' : name_title,
                        'type' : field_type,
                        'base_type' : base_type,
                        'is_optional' : is_optional,
                        'is_repeated' : is_repeated,
                        'is_native' : is_native,
                        'is_enum' : is_enum,
                        'is_bytes' : is_bytes,
                        'is_flag' : is_flag,
                        'proto_name' : proto_field_name, 
                        'proto_type' : proto_type,
                        'idx' : idx,
                        'default_value' : default_value,
                        'namespace' : field_namespace,
                        }
                    idx += 1
                    cur_class['members'].append(cur_member)
                    # print '  ', field_desc.name

        template = env.get_template('types.tmpl')
        f = open(os.path.join(cmdargs.outdir, out_name + '_types.hpp'), 'wt')
        f.write(template.render(args))

        template = env.get_template('proto_hpp.tmpl')
        f = open(os.path.join(cmdargs.outdir, out_name + '_proto.hpp'), 'wt')
        f.write(template.render(args))

        template = env.get_template('proto_cpp.tmpl')
        f = open(os.path.join(cmdargs.outdir, out_name + '_proto.cpp'), 'wt')
        f.write(template.render(args))


parser = argparse.ArgumentParser()
parser.add_argument('--namespace', action='store', default='editor')
parser.add_argument('--outdir', action='store', default='.')
parser.add_argument('--engine', action='store_true')
args = parser.parse_args()

native_types = ENGINE_NATIVE_MESSAGE_TYPES if args.engine else EDITOR_NATIVE_MESSAGE_TYPES
parse_descriptor_set(args, native_types)
