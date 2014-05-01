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
end

function face(indices, idx, a, b, c)
  indices[idx*3+0] = a
  indices[idx*3+1] = b
  indices[idx*3+2] = c
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
      local t = math.sin(3 * 3.1419 * ff)
      local t2 = math.cos(3 * 3.1419 * ff)
      local r = radius * (1 - ff)
      local x = 50 * t^3 + r * c
      local z = 20 * t^4 * t2 + r * s
      angle = angle + inc
      
      vtx(verts, vtx_idx, x, y, z, c, 0, s)
      vtx_idx = vtx_idx + 1
      
      if i < height_segments then
        -- c-d
        -- |\|
        -- a-b
        
        local a = (i + 0) * radial_segments + j
        local b = (i + 0) * radial_segments + (j + 1) % radial_segments
        local c = (i + 1) * radial_segments + j
        local d = (i + 1) * radial_segments + (j + 1) % radial_segments
        
        face(indices, face_idx, a, c, b)
        face_idx = face_idx + 1
        face(indices, face_idx, b, c, d)
        face_idx = face_idx + 1
      end
    end
    
    y = y + height / height_segments
    
  end
end

