import os.path
import os
import time
import glob
import subprocess

# fancy shaders that want their own entry points..
shaders = {
    'fullscreen' : { 'vs' : 'VSQuad', 'ps' : 'PSQuad' }
}

while True:
    for g in glob.glob('*.hlsl'):
        (p, e) = os.path.splitext(g)
        # if the hlsl file is newer than the .vso/.pso file, compile them
        hlsl_time = os.path.getmtime(g)
        vso = p + '.vso'
        pso = p + '.pso'
        vso_time = os.path.getmtime(vso)
        pso_time = os.path.getmtime(pso)
        
        if hlsl_time > vso_time or hlsl_time > pso_time:
            # check if the shader has a special entry point, otherwise use the defaults
            if p in shaders:
                vs = shaders[p]['vs']
                ps = shaders[p]['ps']
            else:
                vs = 'VsMain'
                ps = 'PsMain'
            print 'compiling: ' + g
            # release
            subprocess.call(['fxc', '/Tvs_4_0', '/O3', '/E%s' % vs, '/Fo%s.vso' % p, '/Fc%s.vsa' % p, '%s.hlsl' % p])
            subprocess.call(['fxc', '/Tps_4_0', '/O3', '/E%s' % ps, '/Fo%s.pso' % p, '/Fc%s.psa' % p, '%s.hlsl' % p])
            #debug
            subprocess.call(['fxc', '/Tvs_4_0', '/Od', '/Zi', '/E%s' % vs, '/Fo%sD.vso' % p, '/Fc%sD.vsa' % p, '%s.hlsl' % p])
            subprocess.call(['fxc', '/Tps_4_0', '/Od', '/Zi', '/E%s' % ps, '/Fo%sD.pso' % p, '/Fc%sD.psa' % p, '%s.hlsl' % p])
    time.sleep(1)
