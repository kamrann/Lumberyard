--Gamely:Made it for samples, beginner tutorials and prototypation.

local BindError = "Call Setup First"
local Input = {	
	Pressed = 1,
	Held = 2,
	Released = 3,
	Setup = function(self,table,inputNames)
		if table.Input == nil then table.Input = {} end
		if table.Action == nil then table.Action = {} end
		if table.Input.Listeners == nil then table.Input.Listeners = {} end
		table.Input.table = table
		table.Input.OnPressed = self.Private.OnPressed
		table.Input.OnHeld = self.Private.OnHeld
		table.Input.OnReleased = self.Private.OnReleased
		self.Bind(table,inputNames)
	end,
	Bind = function(table,inputNames)
		assert(table.Input.Listeners ~= nil,BindError)
		for k, v in pairs(inputNames) do
			table.Input.Listeners[v] = InputEventNotificationBus.Connect(table.Input,InputEventNotificationId(v))
			table.Action[v] = Crc32(v)
		end
	end,
	Unbind = function(table,inputNames)
		assert(table.Input.Listeners ~= nil,BindError)
		for k, v in pairs(inputNames) do
			if(table.Input.Listeners[v])then table.Input.Listeners[v]:Disconnect() end
		end
	end,
	UnbindAll = function(table)
		assert(table.Input.Listeners ~= nil,BindError)
		--disconnect all input listeners.
		for k, v in pairs(table.Input.Listeners) do
			if v then
				v:Disconnect()
			end
		end
		table.Input = nil
	end,
	Private = {
		OnPressed = function(self,value) 
			self.table:OnInput(InputEventNotificationBus.GetCurrentBusId().actionNameCrc,1,value)end,
		OnHeld = function(self,value) 
			self.table:OnInput(InputEventNotificationBus.GetCurrentBusId().actionNameCrc,2,value)end,	
		OnReleased = function(self,value) 
			self.table:OnInput(InputEventNotificationBus.GetCurrentBusId().actionNameCrc,3,value)end,
	},
}
return Input