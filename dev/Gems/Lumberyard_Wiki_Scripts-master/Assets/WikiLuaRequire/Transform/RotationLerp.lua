--Gamely:TODO Could be interesting to do a cpp version to make a speed comparison in the wiki.
local RotationLerp = function(a,b,t)
	local two_pi = 6.283185307
	a = (a + two_pi) % two_pi
    b = (b + two_pi) % two_pi

    local diff = math.abs(a - b)
    if(diff > math.pi)then
   		if (a > b)then
            a = a - two_pi
    	elseif (b > a)then
            b = b - two_pi
    	end
	end
	return a + (b - a) * t
end
return RotationLerp