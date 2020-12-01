-- Free Camera Script from Gamely https://github.com/Paolo-Oliverio/Lumberyard_Wiki_Scripts
--TODO 
--Gamepad support and analog input management.
--Unit circle clamping for movement speed.
--Dynamic fov ( replacing the mousewheel behavior )
--Shift and Caps Lock to modify and toggle running e.g. 2x movement speed.

--Imports
local WikiInput = require "WikiLuaRequire.Input.SimpleInput"

--Constants
local halfPi = math.pi * 0.5 

--math.clamp temp fix TODO do it properly elsewere.
math.clamp = function( value, min , max)
	return math.max(math.min(value,max),min)
end

--Script Component Table
local FreeCamera = 
{
	--Editable Properties.
	Properties = {	
		MovementSpeed = {
			Min = {
				default = 1.0,
				description = "Minimum movement speed"},
			Max = {
				default = 100.0,
				description = "Maximum movement speed"},
		},
		--Mouse Sensibility Properties
		Mouse = {
			WheelSensibility = {
				default = 0.01, 
				description = "Mouse wheel movement speed change"},
			Sensibility = {
				default = 0.1,
				description = "Mouse look speed"},
		},
	},
	
	--Entity Bus Functions
	OnActivate = function(self)
		--Connect to tickbus to update every frame.
		self.tickBusHandler = TickBus.Connect(self)
		--Setup WikiInput with actions from input component.
		WikiInput:Setup(self,{"Forward","Strafe","Jaw","Pitch","Wheel"})
		--Instance Working Variables
			--Set movementSpeed halfway between min and max
			self.movementSpeed = (self.Properties.MovementSpeed.Max + self.Properties.MovementSpeed.Min) * 0.5
			--Accumulated Rotation.
			self.rotationAccumulator = Vector2(0,0)
			--Input aquired deltas
			self.movementDelta = Vector2(0,0)
			self.rotationDelta = Vector2(0,0)
	end,
		
	OnDeactivate = function(self)
		self.tickBusHandler:Disconnect()
		WikiInput.UnbindAll(self)
	end,
	
	--TickBus Function
	OnTick = function(self,delta)
		local myId = self.entityId --readability shortcut
		local WorldTransform = TransformBus.Event.GetWorldTM(myId)
		local ForwardAxis = WorldTransform:GetColumn(1)
		local StrafeAxis = WorldTransform:GetColumn(0)
		
		--Update Velocity
			--world velocity vector from local movement vector
			local velocityV3 =  StrafeAxis * self.movementDelta.x + ForwardAxis * self.movementDelta.y
			--Character Physics Apply X and Y with mode 0 and Z with mode 1
			CryCharacterPhysicsRequestBus.Event.RequestVelocity(myId, Vector3(velocityV3.x,velocityV3.y,0),0 )
			CryCharacterPhysicsRequestBus.Event.RequestVelocity(myId, Vector3(0,0,velocityV3.z),1 )
		
		--Update Rotation
			local rot = self.rotationAccumulator
			rot = rot + self.rotationDelta * delta
			--Limit Vertical Rotation in the -90 to 90 range.
			rot.x = math.clamp(rot.x,-halfPi,halfPi)
			rotationDelta = Vector2(0,0)
			self.rotationAccumulator = rot
			--Create a rotation quaternion.(order of quaternion multiplication is very important)
			TransformBus.Event.SetLocalRotationQuaternion(myId,Quaternion.CreateRotationZ(rot.y) * Quaternion.CreateRotationX(rot.x))
	end,
	
	--WikiInput Function
	OnInput = function(self,action,status,value)
		--Check action against values in the table self.Action generated automatically from WikiInput 
		local p = self.Properties
		if action == self.Action.Forward then
			self.movementDelta.y = value * self.movementSpeed
		elseif action == self.Action.Strafe then
			self.movementDelta.x = value * self.movementSpeed
		elseif action == self.Action.Pitch then
			self.rotationDelta.x = value * p.Mouse.Sensibility
		elseif action == self.Action.Jaw then
			self.rotationDelta.y = value * p.Mouse.Sensibility
		elseif action == self.Action.Wheel then
			self.movementSpeed = self.movementSpeed + value * p.Mouse.WheelSensibility
			self.movementSpeed = math.clamp(self.movementSpeed,p.MovementSpeed.Min,p.MovementSpeed.Max)
			--self.Speed = math.max(math.min(self.Speed,p.MovementSpeed.Max ),p.MovementSpeed.Min)
		end
	end,
}
--Return Script Component Table
return FreeCamera