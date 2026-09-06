-- generate_layers.lua (Lua 4.1–compatible)
-- Pure Lua replacements for math functions
PI = 3.1415926535898


-- Simple LCG-based random (if math.random unavailable)
-- Safe random generator for strict Lua 4.1


-- Your map boundary as array of {x,y}
local map_coords = {
  {51734, 74523}, {51170, 75269}, {50667, 77483}, {49914, 78944},
  {48910, 78893}, {47711, 79458}, {46591, 76545}, {45949, 75968},
  {46302, 78655}, {44579, 82146}, {43755, 82774}, {42648, 84427},
  {41313, 86132}, {39736, 87947}, {38943, 94305}, {35842,103657},
  {41666,109988},{44091,115493},{44686,116864},{49764,119644},
  {61975,123739},{70676,101694},{71005, 95243},{70185, 94552},
  {67108, 78410},{62144, 78657},{61694, 78207},
}
function sqrt(x)
  local g = x / 2
  for _ = 1, 10 do g = (g + x / g) / 2 end
  return g
end

-- point-in-polygon by ray‐casting
function point_in_poly(x, y, poly)
  local inside = false
  local n = getn(poly)
  --Msg2Player("point_in_poly x= "..x.." y= "..y)
  for i = 1, n do
    local xi, yi = poly[i][1], poly[i][2]
    local j = (i < n) and (i + 1) or 1
    local xj, yj = poly[j][1], poly[j][2]
    -- explicit edge‐crossing test
	--Msg2Player("point_in_poly i ="..i.." j="..j.."x= "..x.." y= "..y.." xi="..xi.." yi="..yi.." xj="..xj.." yj="..yj)
    if (yi > y and yj <= y) or (yj > y and yi <= y) then
      -- compute x coordinate of intersection
      local xcross = xi + (y - yi) * (xj - xi) / (yj - yi)
	  --Msg2Player("xcross "..xcross)
      if x < xcross then
        inside = not inside
      end
    end
  end
  return inside
  -- if you need 1/0 instead of true/false, use:
  -- return inside and 1 or 0
end

-- random point in bounding box, reject until inside
function random_point_in(poly)
  local minx, maxx = poly[1][1], poly[1][1]
  local miny, maxy = poly[1][2], poly[1][2]
  for i = 2, getn(poly) do
    local p = poly[i]
    if p[1] < minx then minx = p[1] end
    if p[1] > maxx then maxx = p[1] end
    if p[2] < miny then miny = p[2] end
    if p[2] > maxy then maxy = p[2] end
  end

  while true do
    local x = minx + random(minx, maxx)
    local y = miny + random(miny, maxy)
    if point_in_poly(x, y, poly) then
      return x, y
    end
  end
end

-- max distance from (cx,cy) to any vertex
function max_vertex_dist(cx, cy, poly)
  local m = 0
  --Msg2Player("max_vertex_dist poly length "..getn(poly))
  for i = 1, getn(poly) do
    local dx = poly[i][1] - cx
    local dy = poly[i][2] - cy
    local d = sqrt(dx*dx + dy*dy)
	--Msg2Player("max_vertex_dist d = "..d)
    if d > m then m = d end
  end
  return m
end
function sin_approx(x)
  x = x % floor(2 * PI)
  if x > PI then x = x - 2 * PI end
  local x2 = x * x
  return x * (1 - x2 / 6 + x2 * x2 / 120)  -- 3-term Taylor expansion
end

function cos_approx(x)
  return sin_approx(x + PI / 2)
end
-- generate concentric layers of points
function generate_layers(poly, cx, cy, spacing, overshoot, close_r)
dofile("/script/tinhnang/pubg/pubg.lua")
  local Dmax = max_vertex_dist(cx, cy, poly)
  local final_r = Dmax + overshoot
  local layers = {}
  local r = close_r

  while r <= final_r do
    local circ = 2 * PI * r
    local N = max(8, floor(circ / spacing))
	--Msg2Player("generate_layers N = "..N)
    local pts = {}
    for i = 1, N do
      local theta = 2 * PI * (i-1) / N
      local x = cx + r * cos_approx(theta)
      local y = cy + r * sin_approx(theta)
      if point_in_poly(x, y, poly) then
		--Msg2Player("generate_layers point in poly = "..x..y)
        pts[getn(pts)+1] = {x, y}
      end
    end
    if getn(pts) > 0 then
      layers[getn(layers)+1] = { radius = r, points = pts }
    end
    r = r + spacing
  end

  return layers
end