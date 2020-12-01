local Once = {
	create = function(self,component,func)
		local new_inst = {
			component = component,
			func = func,
			OnTick = self.OnTick,
			Reset = self.Reset,
		}
		new_inst.tickHandler = TickBus.Connect(new_inst)
		return new_inst
	end,
	
	Reset = function(self)
		if self.tickHandler == nil then
			self.tickHandler = TickBus.Connect(new_inst)
		end
	end,
	
	OnTick = function(self,deltaTime,scriptTimePoint)
		self.func(self.component)
		self.tickHandler:Disconnect()
		self.tickHandler = nil
	end,
}
return Once