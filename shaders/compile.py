import os.path
import os
import time
import glob
import subprocess

# fancy shaders that want their own entry points..
shaders = {
    'fullscreen' : { 'vs' : 'VSQuad', 'ps' : 'PSQuad' }
}

vsOnly = [ 'quad' ]
psOnly = [ 'copy' ]

while True:
    for g in glob.glob('*.hlsl'):
        (p, e) = os.path.splitext(g)
        vso = p + '.vso'
        pso = p + '.pso'

        compile = True
        hlsl_time = os.path.getmtime(g)
        if p not in psOnly and os.path.isfile(vso):
            # if the hlsl file is newer than the .vso/.pso file, compile them
            vso_time = os.path.getmtime(vso)
            compile = hlsl_time > vso_time

        if p not in vsOnly and os.path.isfile(pso):
            # if the hlsl file is newer than the .vso/.pso file, compile them
            pso_time = os.path.getmtime(pso)
            compile = hlsl_time > pso_time
            
        if compile:
            # check if the shader has a special entry point, otherwise use the defaults
            if p in shaders:
                vs = shaders[p]['vs']
                ps = shaders[p]['ps']
            else:
                vs = 'VsMain'
                ps = 'PsMain'
            print 'compiling: ' + g
            
            # release
            if not p in psOnly:
                subprocess.call(['fxc', '/Tvs_5_0', '/O3', '/E%s' % vs, '/Fo%s.vso' % p, '/Fc%s.vsa' % p, '%s.hlsl' % p])
            if not p in vsOnly:
                subprocess.call(['fxc', '/Tps_5_0', '/O3', '/E%s' % ps, '/Fo%s.pso' % p, '/Fc%s.psa' % p, '%s.hlsl' % p])
                
            #debug
            if not p in psOnly:
                subprocess.call(['fxc', '/Tvs_5_0', '/Od', '/Zi', '/E%s' % vs, '/Fo%sD.vso' % p, '/Fc%sD.vsa' % p, '%s.hlsl' % p])
            if not p in vsOnly:
                subprocess.call(['fxc', '/Tps_5_0', '/Od', '/Zi', '/E%s' % ps, '/Fo%sD.pso' % p, '/Fc%sD.psa' % p, '%s.hlsl' % p])
    time.sleep(1)
