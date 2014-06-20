import os.path
import os
import time
import glob
import subprocess

# a single .hlsl can contain multiple entry points
vs_entry = {
}

ps_entry = {
    'tonemap' : ['LuminanceMap', 'Composite', 'AdaptLuminance']
}

vs_only = [ 'quad' ]
ps_only = [ 'copy', 'tonemap' ]

def gen_filenames(cur, only, default_entry_point, entry_point_list, ext):
    if cur in only:
        return []

    res = []
    ss = [default_entry_point] if not cur in entry_point_list else entry_point_list[cur]
    for s in ss:
        res.append(cur + '_' + s + ext)
    return res

while True:
    for g in glob.glob('*.hlsl'):
        (cur, e) = os.path.splitext(g)
        vso = gen_filenames(cur, ps_only, 'VsMain', vs_entry, '.vso')
        pso = gen_filenames(cur, vs_only, 'PsMain', ps_entry, '.pso')

        compile = True
        hlsl_time = os.path.getmtime(g)
        for v in vso:
            # if the hlsl file is newer than the .vso/.pso file, compile them
            try:
                vso_time = os.path.getmtime(v)
                compile = hlsl_time > vso_time
            except:
                compile = True

        for p in pso:
            # if the hlsl file is newer than the .vso/.pso file, compile them
            try:
                pso_time = os.path.getmtime(p)
                compile = hlsl_time > pso_time
            except:
                compile = True
            
        if compile:
            # check if the current shader has multiple (or non default) entry points
            vs = ['VsMain'] if not cur in vs_entry else vs_entry[cur]
            ps = ['PsMain'] if not cur in ps_entry else ps_entry[cur]

            # vertex shaders
            if not cur in ps_only:
                for v in vs:
                    out_name = cur + '_' + v
                    # debug, then release
                    subprocess.call(['fxc', '/Tvs_5_0', '/Od', '/Zi', ('/E%s' % v), ('/Fo%sD.vso' % out_name), ('/Fc%sD.vsa' % out_name), ('%s.hlsl' % cur)])
                    subprocess.call(['fxc', '/Tvs_5_0', '/O3',        ('/E%s' % v), ('/Fo%s.vso' % out_name),  ('/Fc%s.vsa' % out_name),  ('%s.hlsl' % cur)])
                    
            # pixel shaders
            if not cur in vs_only:
                for p in ps:
                    out_name = cur + '_' + p
                    subprocess.call(['fxc', '/Tps_5_0', '/Od', '/Zi', ('/E%s' % p), ('/Fo%sD.pso' % out_name), ('/Fc%sD.psa' % out_name), ('%s.hlsl' % cur)])
                    subprocess.call(['fxc', '/Tps_5_0', '/O3',        ('/E%s' % p), ('/Fo%s.pso' % out_name),  ('/Fc%s.psa' % out_name),  ('%s.hlsl' % cur)])
    time.sleep(1)
