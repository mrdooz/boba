-- spikey cylinder type thing :)

require('math')

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


function generate(radius, height, radial_segments, height_segments)
  verts = get_vertices()
  indices = get_indices()
  
  local y = 0
  local vtx_idx = 0
  local face_idx = 0
  for i = 0, height_segments, 1 do

    local inc = 2 * 3.1415926 / radial_segments
    local angle = 0
    for j = 0, radial_segments - 1, 1 do
      -- create a ring
      local c = math.cos(angle)
      local s = math.sin(angle)
      local ff = i / height_segments
      local t = 0.25 * (2 + math.sin(3 * 3.1419 * ff))
      local t2 = 0.25 * (2 + math.cos(3 * 3.1419 * ff))
      local r = radius * (2 + math.sin(5 * (1 - ff)))
      local x = 5 * t^3 * r * c
      local z = 2 * math.random() * t^5 * t2 * r * s
      angle = angle + inc
      
      local n = v_normalize(v_sub(v_create(x, y, z), v_create(0, y, 0)))
      --vtx(verts, vtx_idx, x, y, z, n.x, n.y, n.z)
      local vx = r * c
      local vy = y
      local vz = r * s
   
      vtx_idx = vtx(verts, vtx_idx, vx, vy, vz, c, 0, s)
      
      if i < height_segments then
        -- c-d
        -- |\|
        -- a-b
        local a = (i + 0) * radial_segments + j
        local b = (i + 0) * radial_segments + (j + 1) % radial_segments
        local c = (i + 1) * radial_segments + j
        local d = (i + 1) * radial_segments + (j + 1) % radial_segments

        face_idx = face(indices, face_idx, a, c, b)
        face_idx = face(indices, face_idx, b, c, d)
      end
    end
    
    y = y + height / height_segments
  end
  
  for i = 0, height_segments - 1, 1 do
    for j = 0, radial_segments - 1, 1 do
    
      local a = (i + 0) * radial_segments + j
      local b = (i + 0) * radial_segments + (j + 1) % radial_segments
      local c = (i + 1) * radial_segments + j
      local d = (i + 1) * radial_segments + (j + 1) % radial_segments
      
      local va = v_verts(verts, a)
      local vb = v_verts(verts, b)
      local vc = v_verts(verts, c)
      local vd = v_verts(verts, d)
      
      -- create the face normal
      local vn = v_normalize(v_cross(v_sub(vb, va), v_sub(vc, va)))
      local vn2 = v_scale(5 + 10 * math.random(), vn)
      
      -- place the new vertex along the face normal
      local m = v_add(v_scale(1.0/4, v_add(va, vb, vc, vd)), vn2)

      local e = vtx_idx
      vtx_idx = add_vtx(verts, vtx_idx, m, vn2)

      face_idx = face(indices, face_idx, a, b, e)
      face_idx = face(indices, face_idx, b, d, e)
      face_idx = face(indices, face_idx, d, c, e)
      face_idx = face(indices, face_idx, c, a, e)
    end
    
  end
  
end

