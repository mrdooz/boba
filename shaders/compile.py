import os.path
import os
import time
import glob
import subprocess
import collections

vs = collections.defaultdict(list)

ps = collections.defaultdict(list)
ps['text_shader'] = ['EdgeDetect']
ps['tonemap'] = ['LuminanceMap', 'Composite', 'AdaptLuminance', 'BloomThreshold']

default_vs = ['quad', 'debug_draw', 'fullscreen', 'generator', 'text_shader', 'particle']
default_ps = ['debug_draw', 'fullscreen', 'generator', 'text_shader', 'copy', 'particle']

cs = {
    'blur' : ['CopyTranspose', 'BlurTranspose'],
}

for x in default_vs:
    vs[x].append('VsMain')

for x in default_ps:
    ps[x].append('PsMain')

first_run = True

def time_cmp(t, f):
    global first_run
    if first_run:
        return True
    try:
        obj_time = os.path.getmtime(f)
        return t > obj_time
    except:
        return True

while True:
    #import pdb; pdb.set_trace()
    for g in glob.glob('*.hlsl'):
        (cur, e) = os.path.splitext(g)

        hlsl_time = os.path.getmtime(g)

        # vertex shaders
        if cur in vs:
            objs = vs[cur]
            if time_cmp(hlsl_time, cur + '_' + objs[0] + '.vso'):
                for v in objs:
                    out_name = cur + '_' + v
                    subprocess.call(['fxc', '/Tvs_5_0', '/Od', '/Zi', ('/E%s' % v), ('/Fo%sD.vso' % out_name), ('/Fc%sD.vsa' % out_name), ('%s.hlsl' % cur)])
                    subprocess.call(['fxc', '/Tvs_5_0', '/O3',        ('/E%s' % v), ('/Fo%s.vso' % out_name),  ('/Fc%s.vsa' % out_name),  ('%s.hlsl' % cur)])

        # pixel shaders
        if cur in ps:
            objs = ps[cur]
            if time_cmp(hlsl_time, cur + '_' + objs[0] + '.pso'):
                for p in objs:
                    out_name = cur + '_' + p
                    subprocess.call(['fxc', '/Tps_5_0', '/Od', '/Zi', ('/E%s' % p), ('/Fo%sD.pso' % out_name), ('/Fc%sD.psa' % out_name), ('%s.hlsl' % cur)])
                    subprocess.call(['fxc', '/Tps_5_0', '/O3',        ('/E%s' % p), ('/Fo%s.pso' % out_name),  ('/Fc%s.psa' % out_name),  ('%s.hlsl' % cur)])

        # compute shaders
        if cur in cs:
            objs = cs[cur]
            if time_cmp(hlsl_time, cur + '_' + objs[0] + '.cso'):
                for c in objs:
                    out_name = cur + '_' + c
                    subprocess.call(['fxc', '/Tcs_5_0', '/Od', '/Zi', ('/E%s' % c), ('/Fo%sD.cso' % out_name), ('/Fc%sD.csa' % out_name), ('%s.hlsl' % cur)])
                    subprocess.call(['fxc', '/Tcs_5_0', '/O3',        ('/E%s' % c), ('/Fo%s.cso' % out_name),  ('/Fc%s.csa' % out_name),  ('%s.hlsl' % cur)])

    first_run = False
    time.sleep(1)
