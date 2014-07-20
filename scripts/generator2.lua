mesh = require('mesh')
meshlib = require('meshlib')

function get_y(x, z, y_values)
  -- check if the y value for this x/z is known, otherwise
  -- create it
  local key = tostring(x) .. '|' .. tostring(z)
  local y = y_values[key]
  if y == nil then
    y = 35 * math.random()
    y_values[key] = y
  end
  return y
end

function generate(width, height, width_segments, height_segments)
  verts = get_vertices()
  indices = get_indices()

  local vtx_idx = 0
  local face_idx = 0

  local y_values = {}
  local y = 0
  local z = 0
  local x_inc = width / width_segments
  local z_inc = height / height_segments
  for i = 0, height_segments - 1 do
    local x = 0
    for j = 0, width_segments - 1 do
    
      -- create the 2 tris at the current position
      -- c-d
      -- |\|
      -- a-b
      local xa = x;         za = z;
      local xb = x + x_inc; zb = z;
      local xc = x;         zc = z + z_inc;
      local xd = x + x_inc; zd = z + z_inc;
      
      local va = v_create(xa, get_y(xa, za, y_values), za)
      local vb = v_create(xb, get_y(xb, zb, y_values), zb)
      local vc = v_create(xc, get_y(xc, zc, y_values), zc)
      local vd = v_create(xd, get_y(xd, zd, y_values), zd)
      
      local n0 = v_normalize(v_cross(v_sub(vc, va), v_sub(vb, va)))
      local n1 = v_normalize(v_cross(v_sub(vb, vd), v_sub(vc, vd)))
      
      local v0 = vtx_idx
      local v1 = add_vtx(verts, v0, va, n0)
      local v2 = add_vtx(verts, v1, vc, n0)
      local v3 = add_vtx(verts, v2, vb, n0)
      local v4 = add_vtx(verts, v3, vb, n1)
      local v5 = add_vtx(verts, v4, vc, n1)
      vtx_idx  = add_vtx(verts, v5, vd, n1)

      face_idx = face(indices, face_idx, v0, v1, v2)
      face_idx = face(indices, face_idx, v3, v4, v5)

      x = x + x_inc
    end
    z = z + z_inc
  end
end
