function v_create(x, y, z)
  local v = {}
  v['x'] = x
  v['y'] = y
  v['z'] = z
  return v
end

function v_length(v)
  local dx = v.x * v.x
  local dy = v.y * v.y
  local dz = v.z * v.z
  return math.sqrt(dx+dy+dz)
end

function v_sub(a, b)
  return v_create(a.x - b.x, a.y - b.y, a.z - b.z)
end

function v_add(...)
  local vx = 0
  local vy = 0
  local vz = 0
  for i, v in pairs({...}) do
    vx = vx + v.x
    vy = vy + v.y
    vz = vz + v.z
  end
  return v_create(vx, vy, vz)
end

function v_cross(a, b)
  return v_create(
    a.z * b.y - a.y * b.z,
    a.x * b.z - a.z * b.x,
    a.y * b.x - a.x * b.y)
end

function v_normalize(v)
  local len = v_length(v)
  if len == 0 then
    return v
  end
  local recip_len = 1 / len
  return v_create(v.x * recip_len, v.y * recip_len, v.z * recip_len)
end

function v_scale(s, v)
  return v_create(s * v.x, s * v.y, s * v.z)
end


function vtx(verts, idx, v0, v1, v2, n0, n1, n2)
  -- set pos
  verts[idx*6+0] = v0
  verts[idx*6+1] = v1
  verts[idx*6+2] = v2
  
  -- set normal
  verts[idx*6+3] = n0
  verts[idx*6+4] = n1
  verts[idx*6+5] = n2
  
  return idx + 1
end

function add_vtx(verts, idx, v, n)
  -- set pos
  verts[idx*6+0] = v.x
  verts[idx*6+1] = v.y
  verts[idx*6+2] = v.z
  
  -- set normal
  verts[idx*6+3] = n.x
  verts[idx*6+4] = n.y
  verts[idx*6+5] = n.z
  
  return idx + 1
end

function face(indices, idx, a, b, c)
  indices[idx*3+0] = a
  indices[idx*3+1] = b
  indices[idx*3+2] = c
  
  return idx + 1
end

function v_verts(verts, idx)
  return v_create(verts[idx*6+0], verts[idx*6+1], verts[idx*6+2])
end
