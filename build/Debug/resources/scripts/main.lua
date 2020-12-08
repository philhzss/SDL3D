local test = require("test")

local game = getGame()

local resourceManager = game:getResourceManager()
local inputManager = game:getInputManager()
local entityManager = game:getEntityManager()

local musicButtonPressedLastFrame = false
local mouseCaptureButtonPressedLastFrame = false

local lastMousePos = Vec2(-1, -1)
local lastRelMousePos = Vec2(-1, -1)

function gameInit()
	--lastMousePos.x = (lastMousePos.x / game:getSize().x * 2) - 1
	--lastMousePos.y = (lastMousePos.y / game:getSize().y * 2) - 1
	test.foo()
	Utils.logprint("Hello, init from lua!")
end

function gameStep()
	local game = getGame()
	
	local camera = entityManager:getGameCamera()
	
	local shader = resourceManager:findShader("basic");
	local objects = entityManager:getObjects()
	
	doControls();
	
	for i,v in ipairs(objects) do
		local physicsBody = v:getPhysicsBody()
		physicsBody:renderDebugShape(shader, camera)
	end
	
	local buildingPosition = test.building:getPhysicsBody():getPosition()
	test.building:getPhysicsBody():setPosition(Vec3(buildingPosition.x, 0, 0))
	
	local rotation = test.firstMonkey:getPhysicsBody():getRotation()
	test.firstMonkey:getPhysicsBody():setRotation(Vec3(rotation.x + 0.1, rotation.y, rotation.z))
end

-- http://www.scs.ryerson.ca/~danziger/mth141/Handouts/Slides/projections.pdf
function projectVec2OnVec2(vector, projectionVector)
	local projected = Vec2.scalarMul(projectionVector, (  Vec2.dot(vector, projectionVector) / Vec2.length(projectionVector)  ))
	return projected
end

function doControls()
	local speed = 0.4
	local angleIncrementation = 0.02
	
	local camera = entityManager:getGameCamera()
	local cameraPhysicsBody = camera:getPhysicsBody()

	

	
	cameraPhysicsBody:renderDebugShapeWithCoord(resourceManager:findShader("basic"), camera, 0.0)

	-- Movement
	if(inputManager:isKeyPressed(KeyCode.LSHIFT)) then
		speed = speed * 10
	end
	
	if(inputManager:isKeyPressed(KeyCode.w)) then
		local cameraDirection = camera:getDirection()
		
		 -- Normalize to guarantee that it is the same everywhere
		local velocity = Vec3.scalarMul(Vec3.normalize(Vec3(cameraDirection.x, 0, cameraDirection.z)), speed)
		cameraPhysicsBody:setVelocity(velocity)
	elseif(inputManager:isKeyPressed(KeyCode.s)) then
		local cameraDirection = camera:getDirection()
	
		local velocity = Vec3.scalarMul(Vec3.normalize(Vec3(-cameraDirection.x, 0, -cameraDirection.z)), speed)
		cameraPhysicsBody:setVelocity(velocity)
	end
	
	if(inputManager:isKeyPressed(KeyCode.a) or inputManager:isKeyPressed(KeyCode.d)) then
		local sidewaysVelocityAngle = 1.5708
		
		if(inputManager:isKeyPressed(KeyCode.a)) then
			sidewaysVelocityAngle = -sidewaysVelocityAngle
		end
		
		local normalizedCameraDirection = Vec4.normalize(camera:getDirection())
		local cameraVelocity = cameraPhysicsBody:getVelocity()
		
		local otherX = (normalizedCameraDirection.x * math.cos(sidewaysVelocityAngle) - normalizedCameraDirection.z * math.sin(sidewaysVelocityAngle)) * speed
		local otherZ = (normalizedCameraDirection.x * math.sin(sidewaysVelocityAngle) + normalizedCameraDirection.z * math.cos(sidewaysVelocityAngle)) * speed
		
		-- Get the velocity projection on the camera direction (this lets us keep vertical speed)
		local cameraVelocity2D = Vec2(cameraVelocity.x, cameraVelocity.z)
		local cameraDirection2D = Vec2(normalizedCameraDirection.x, normalizedCameraDirection.z)
		local projected = projectVec2OnVec2(cameraVelocity2D, cameraDirection2D)
		
		local currentVelocity = cameraPhysicsBody:getVelocity()
		
		-- Make sure we don't indefinitely add velocity, so normalize to our speed (only keep direction)
		local newVelocity = Vec3(otherX + projected.x,
								currentVelocity.y,
								otherZ + projected.y)
		
		cameraPhysicsBody:setVelocity(newVelocity)
	end
	

	-- View controls
	local currentMousePos = inputManager:getMousePos()
	local currentMouseRelPos = inputManager:getMouseRelPos()	

	
	-- Update the mouse pos to be between -1 and +1
	currentMousePos.x = (currentMousePos.x / game:getSize().x * 2) - 1
	currentMousePos.y = (currentMousePos.y / game:getSize().y * 2) - 1
	
	--Utils.logprint("LUA: lastMousePos: " .. tostring(lastMousePos.x) .. ", " .. tostring(lastMousePos.y))
  	Utils.logprint("LUA: currentMousePos: " .. tostring(currentMousePos.x) .. ", " .. tostring(currentMousePos.y))
	Utils.logprint("LUA: currentMouseRelPos: " .. tostring(currentMouseRelPos.x) .. ", " .. tostring(currentMouseRelPos.y))

	if(game:getRelativeMouseMode()) then
		if((lastRelMousePos.x ~= currentMouseRelPos.x) or (lastRelMousePos.y ~= currentMouseRelPos.y)) then
  			Utils.logprint("LUA: Mouse moved!! Relative pos diff: " .. tostring(currentMouseRelPos.x) .. ", " .. tostring(currentMouseRelPos.y))
			--Utils.logprint("LUA: Mouse lastPos: " .. tostring(lastMousePos.x) .. ", " .. tostring(lastMousePos.y))
		
			local cameraDirection = camera:getDirection() -- Here we make sure we have the latest direction

			Utils.logprint("LUA: CameraDirection BEFORE: " .. tostring(camera:getDirection().x) .. ", " .. tostring(camera:getDirection().y) .. ", " .. tostring(camera:getDirection().z))

			local relDifVect = Vec4(0, -currentMouseRelPos.y/50, currentMouseRelPos.x/50, 0)

			local newVect = Vec4(0,0,0,0) -- init?
			newVect.x = relDifVect.x + cameraDirection.x
			newVect.y = relDifVect.y + cameraDirection.y
			newVect.z = relDifVect.z + cameraDirection.z
			newVect.w = relDifVect.w + cameraDirection.w -- its 0 but who cares

			camera:setDirection(newVect)

			Utils.logprint("LUA: CameraDirection AFTER: " .. tostring(camera:getDirection().x) .. ", " .. tostring(camera:getDirection().y) .. ", " .. tostring(camera:getDirection().z))
	
		else
		   --Utils.logprint("LUA: Mouse did not move: " .. tostring(currentMousePos.x) .. ", " .. tostring(currentMousePos.y))
		   --Utils.logprint("LUA: Mouse lastPos: " .. tostring(lastMousePos.x) .. ", " .. tostring(lastMousePos.y))
		end
	
		lastMousePos.x = currentMousePos.x
		lastMousePos.y = currentMousePos.y
		lastRelMousePos.x = currentMouseRelPos.x
		lastRelMousePos.y = currentMouseRelPos.y
		Utils.logprint("")
	end


	if(inputManager:isKeyPressed(KeyCode.UP)) then
		local cameraDirection = camera:getDirection() -- Here we make sure we have the latest direction
		
		-- Base of the triangle
		local base = math.sqrt((cameraDirection.x * cameraDirection.x) + (cameraDirection.z * cameraDirection.z))
	
		-- http://stackoverflow.com/questions/22818531/how-to-rotate-2d-vector
		local newBase = base * math.cos(angleIncrementation) - cameraDirection.y * math.sin(angleIncrementation)
		local newY = base * math.sin(angleIncrementation) + cameraDirection.y * math.cos(angleIncrementation)
		
		local ratio = newBase / base
		
		camera:setDirection(Vec4(cameraDirection.x * ratio, newY, cameraDirection.z * ratio, 0)) -- 0 for vector
	elseif(inputManager:isKeyPressed(KeyCode.DOWN)) then
		local cameraDirection = camera:getDirection()
		
		-- Base of the triangle
		local base = math.sqrt((cameraDirection.x * cameraDirection.x) + (cameraDirection.z * cameraDirection.z))
	
		local newBase = base * math.cos(-angleIncrementation) - cameraDirection.y * math.sin(-angleIncrementation)
		local newY = base * math.sin(-angleIncrementation) + cameraDirection.y * math.cos(-angleIncrementation)
		
		local ratio = newBase / base
		
		camera:setDirection(Vec4(cameraDirection.x * ratio, newY, cameraDirection.z * ratio, 0)) -- 0 for vector
	end
	
	if(inputManager:isKeyPressed(KeyCode.LEFT)) then
		local cameraDirection = camera:getDirection()
		
		local newX = cameraDirection.x * math.cos(-angleIncrementation) - cameraDirection.z * math.sin(-angleIncrementation)
		local newZ = cameraDirection.x * math.sin(-angleIncrementation) + cameraDirection.z * math.cos(-angleIncrementation)
		
		camera:setDirection(Vec4(newX, cameraDirection.y, newZ, 0)) -- 0 for vector
	elseif(inputManager:isKeyPressed(KeyCode.RIGHT)) then
		local cameraDirection = Vec3.fromVec4(camera:getDirection())
		
		local newX = cameraDirection.x * math.cos(angleIncrementation) - cameraDirection.z * math.sin(angleIncrementation)
		local newZ = cameraDirection.x * math.sin(angleIncrementation) + cameraDirection.z * math.cos(angleIncrementation)
		
		camera:setDirection(Vec4(newX, cameraDirection.y, newZ, 0)) -- 0 for vector
	end
	Utils.logprint("LUA: CameraDirection check: " .. tostring(camera:getDirection().x) .. ", " .. tostring(camera:getDirection().y) .. ", " .. tostring(camera:getDirection().z))
	Utils.logprint("")
	Utils.logprint("")
	Utils.logprint("")
	
	-- Up/down controls
	if(inputManager:isKeyPressed(KeyCode.SPACE)) then
		cameraPhysicsBody:setPosition( Vec3.add(cameraPhysicsBody:getPosition(), Vec3(0, speed/30, 0)) )
	elseif(inputManager:isKeyPressed(KeyCode.LCTRL)) then
		cameraPhysicsBody:setPosition( Vec3.add(cameraPhysicsBody:getPosition(), Vec3(0, -speed/30, 0)) )
	end
	
	-- Music controls
	if(inputManager:isKeyPressed(KeyCode.m)) then
		if(musicButtonPressedLastFrame == false) then
			musicButtonPressedLastFrame = true
			
			resourceManager:findSound("soundEffect"):play()
			
			local music = resourceManager:findSound("music")
			if(music:isPaused()) then
				music:resume()
			else
				music:pause()
			end
		end
	else
		musicButtonPressedLastFrame = false
	end

	-- Mouse capture controls
	if(inputManager:isKeyPressed(KeyCode.o)) then
		if(mouseCaptureButtonPressedLastFrame == false) then
			mouseCaptureButtonPressedLastFrame = true

			resourceManager:findSound("soundEffect"):play()

			if(game:getRelativeMouseMode()) then
				game:setRelativeMouseMode(false)
				Utils.logprint("LUA: Mouse released!!")
			else
				game:setRelativeMouseMode(true)
				Utils.logprint("LUA: Mouse captured")
			end
		end
	else
		mouseCaptureButtonPressedLastFrame = false
	end
end
