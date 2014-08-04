import descriptor_pb2
from jinja2 import Environment, PackageLoader, Template

# list of types that we have 'native' support for, ie To/FromProtocol already exist
NATIVE_MESSAGE_TYPES = set([
	'Vector2',
	'Vector3'
])

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

env = Environment(loader=PackageLoader('codegen', 'templates'))

fds = descriptor_pb2.FileDescriptorSet()
fds.ParseFromString(open('effects.desc', 'rb').read())

args = { 'classes' : [], 'all_classes' : [] }

# iterate over FileDescriptor
for file_desc in fds.file:
	# print file_desc.name

	# iterator over Descriptor (messages)
	for msg_desc in file_desc.message_type:

		args['all_classes'].append(msg_desc.name)

		if msg_desc.name in NATIVE_MESSAGE_TYPES:
			continue

		cur_class = { 'members' : [], 'enums' : []}
		cur_class['name'] = msg_desc.name
		args['classes'].append(cur_class)

		# create the enums
		for enum in msg_desc.enum_type:
			e = { 'name' : enum.name, 'vals' : []}
			cur_class['enums'].append(e)
			for value in enum.value:
				e['vals'].append({'name' : value.name, 'number' : value.number})

		for field_desc in msg_desc.field:
			type_number = field_desc.type

			is_msg = type_number == 11
			is_optional = field_desc.label == 1
			is_required = field_desc.label == 2
			is_repeated = field_desc.label == 3
			is_enum = type_number == 14

			if type_number in NATIVE_TYPES:
				type_name = NATIVE_TYPES[type_number]
			elif is_msg or is_enum:
				s = field_desc.type_name.split('.')
				# convert .effect.plexus.NoiseEffector.ApplyTo to ApplyTo
				type_name = field_desc.type_name.split('.')[-1]

			# if repeated, convert to 'vector<type_name>'
			if is_repeated:
				type_name = 'vector<%s>' % type_name

			cur_member = { 'name' : field_desc.name, 'type' : type_name }
			cur_class['members'].append(cur_member)
			# print '  ', field_desc.name
	# exit(1)

template = env.get_template('editor.tmpl')
print template.render(args)