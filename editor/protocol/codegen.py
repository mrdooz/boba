import descriptor_pb2
from jinja2 import Environment, PackageLoader, Template

EXCLUDE_MESSAGES = set([
	'Color4',
	'Settings',
	'Matrix3x3',
	'Matrix4x4',
	])

NATIVE_MESSAGE_TYPES = {
	'Vector2' : 'Vector2f',
	'Vector3' : 'Vector3f',
	'Vector4' : 'Vector4f',	
}

NATIVE_TYPES = {
	1 	: 'double',
	2	: 'float',
	3	: 'int64_t',
	4	: 'uint64_t',
	5	: 'int32_t',
	6	: 'int64_t',
	7	: 'int32_t',
	8	: 'bool',
	9	: 'string',
	13	: 'uint32_t',
	15	: 'int32_t',
	16 	: 'int64_t',
	17	: 'int32_t',
	18	: 'int64_t',
}

def underscore_to_camel_case(str):
	# 'under_score_hej' -> 'underScoreHej'
	x = str.split('_')
	return x[0] + ''.join(map(lambda x : x.title(), x[1:]))

env = Environment(loader=PackageLoader('codegen', 'templates'))

fds = descriptor_pb2.FileDescriptorSet()
fds.ParseFromString(open('effects.desc', 'rb').read())

args = { 'classes' : [], 'all_classes' : [] }

# iterate over FileDescriptor
for file_desc in fds.file:
	# print file_desc.name

	package_namespace = '::'.join(file_desc.package.split('.'))

	# iterator over Descriptor (messages)
	for msg_desc in file_desc.message_type:

		if msg_desc.name in EXCLUDE_MESSAGES:
			continue

		args['all_classes'].append(msg_desc.name)

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
		for field_desc in msg_desc.field:
			type_number = field_desc.type

			is_msg = type_number == 11
			is_optional = field_desc.label == 1
			is_required = field_desc.label == 2
			is_repeated = field_desc.label == 3
			is_enum = type_number == 14

			proto_type = field_desc.type_name

			if type_number in NATIVE_TYPES:
				field_type = NATIVE_TYPES[type_number]
			elif is_msg:
				s = field_desc.type_name.split('.')
				proto_type = ''.join(s[1:])
				suffix = field_desc.type_name.split('.')[-1]
				# use the native message type if one exists
				field_type = NATIVE_MESSAGE_TYPES.get(suffix, suffix)
			elif is_enum:
				s = field_desc.type_name.split('.')
				# convert .effect.plexus.NoiseEffector.ApplyTo to NoiseEffector::ApplyTo
				outer = field_desc.type_name.split('.')[-2]
				field_type = outer + '::' + field_desc.type_name.split('.')[-1]

			# if repeated, convert to 'vector<type_name>'
			if is_repeated:
				field_type = 'vector<%s>' % field_type

			field_name = underscore_to_camel_case(field_desc.name)

			cur_member = { 
				'name' : field_name, 
				'type' : field_type,
				'is_repeated' : is_repeated,
				'is_native' : not is_msg,
				'proto_type' : proto_type,
				}
			cur_class['members'].append(cur_member)
			# print '  ', field_desc.name
	# exit(1)

template = env.get_template('editor.tmpl')

f = open('effects_proto.hpp', 'wt')
f.write(template.render(args))