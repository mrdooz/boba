import common_pb2
import descriptor_pb2
from jinja2 import Environment, PackageLoader, Template

EXCLUDE_MESSAGES = set([
    'Color4',
#    'Settings',
    'Matrix3x3',
    'Matrix4x4',
    ])

NATIVE_MESSAGE_TYPES = {
    'Vector2'   : 'Vector2f',
    'Vector3'   : 'Vector3f',
    'Vector4'   : 'Vector4f', 
    'Color4'    : 'Color', 
}

FORWARD_DECL = {
#    'Vector3f FromProtocol(const common::Vector3& p);',
}

NATIVE_TYPES = {
    1   : 'double',
    2   : 'float',
    3   : 'int64_t',
    4   : 'uint64_t',
    5   : 'int32_t',
    6   : 'int64_t',
    7   : 'int32_t',
    8   : 'bool',
    9   : 'string',
    13  : 'uint32_t',
    15  : 'int32_t',
    16  : 'int64_t',
    17  : 'int32_t',
    18  : 'int64_t',
}

def underscore_to_camel_case(str):
    # 'under_score_hej' -> 'underScoreHej'
    x = str.split('_')
    return x[0] + ''.join(map(lambda x : x.title(), x[1:]))

def underscore_to_title_case(str):
    # 'under_score_hej' -> 'underScoreHej'
    x = str.split('_')
    return ''.join(map(lambda x : x.title(), x))

env = Environment(
    loader=PackageLoader('codegen', 'templates'),
    trim_blocks = True,
    lstrip_blocks = True)

fds = descriptor_pb2.FileDescriptorSet()
fds.ParseFromString(open('effects.desc', 'rb').read())

args = { 'classes' : [], 'all_classes' : [] }

# iterate over FileDescriptor
for file_desc in fds.file:
    # print file_desc.name

    package_namespace = '::'.join(file_desc.package.split('.'))

    # iterator over Descriptor (messages)
    for msg_desc in file_desc.message_type:
        # print msg_desc.name, msg_desc.options, dir(msg_desc.options)
        # if isinstance(msg_desc, descriptor_pb2.DescriptorProto):
        #     print msg_desc.name, msg_desc.options.HasExtension(common_pb2.context)

        if msg_desc.name in EXCLUDE_MESSAGES:
            continue

        args['all_classes'].append(msg_desc.name)
        args['forward_decls'] = FORWARD_DECL

        # don't create structs for message types we have native implementations for
        if msg_desc.name in NATIVE_MESSAGE_TYPES:
            continue

        cur_class = { 
            'members' : [], 
            'enums' : [],
            'proto_type' : package_namespace + '::' + msg_desc.name,
            }

        cur_class['name'] = msg_desc.name
        args['classes'].append(cur_class)

        # extract any enums
        for enum in msg_desc.enum_type:
            e = { 'name' : enum.name, 'vals' : []}
            cur_class['enums'].append(e)
            for value in enum.value:
                e['vals'].append({'name' : value.name, 'number' : value.number})

        # process fields
        idx = 0
        for field_desc in msg_desc.field:
            type_number = field_desc.type
            # type_name = .effect.protocol.plexus.NoiseEffector.ApplyTo
            # import pdb; pdb.set_trace()

            is_msg = type_number == 11
            is_bytes = type_number == 12
            is_enum = type_number == 14
            is_native = type_number in NATIVE_TYPES
            is_optional = field_desc.label == 1
            is_required = field_desc.label == 2
            is_repeated = field_desc.label == 3

            proto_type = field_desc.type_name
            default_value = None
            if len(field_desc.default_value) > 0:
                # handle special cases for default value
                if is_enum:
                    default_value = field_desc.type_name.split('.')[-1] + '::' + field_desc.default_value
                else:
                    default_value = field_desc.default_value

            if type_number in NATIVE_TYPES:
                field_type = NATIVE_TYPES[type_number]
                base_type = field_type
            elif is_msg:
                s = field_desc.type_name.split('.')
                proto_type = ''.join(s[1:])
                suffix = field_desc.type_name.split('.')[-1]
                # use the native message type if one exists
                field_type = NATIVE_MESSAGE_TYPES.get(suffix, suffix)
                base_type = field_type
            elif is_enum:
                s = field_desc.type_name.split('.')
                # convert .effect.plexus.NoiseEffector.ApplyTo to NoiseEffector::ApplyTo
                outer = field_desc.type_name.split('.')[-2]
                field_type = outer + '::' + field_desc.type_name.split('.')[-1]
                base_type = field_type
            elif is_bytes:
                field_type = 'std::vector<uint8_t>'
            else:
                print '** UNHANDLED TYPE: ', type_number

            # if repeated, convert to 'vector<type_name>'
            if is_repeated:
                base_type = field_type
                field_type = 'vector<%s>' % field_type

            proto_field_name = field_desc.name
            field_name = underscore_to_camel_case(field_desc.name)
            name_title = underscore_to_title_case(field_desc.name)

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
                'proto_name' : proto_field_name, 
                'proto_type' : proto_type,
                'idx' : idx,
                'default_value' : default_value,
                }
            idx += 1
            cur_class['members'].append(cur_member)
            # print '  ', field_desc.name
    # exit(1)

template = env.get_template('editor.tmpl')

f = open('effects_proto.hpp', 'wt')
f.write(template.render(args))