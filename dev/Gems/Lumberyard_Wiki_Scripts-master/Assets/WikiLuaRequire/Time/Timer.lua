--Gamely: Lua only Tick bus based Timer sample.
local paused,play,stop=0,1,2
local TimerFunctions = {
	Restart = function(self)
		self.timeout = self.duration
		self.state = play
		remainingLoops = loops
		if self.TickHandler == nil then
			self.TickHandler = TickBus.Connect(self)
		end
	end,
	Stop = function(self)--remember to stop before to set instance to nil. 
		self.state = stop
		Debug.Error(self.TickHandler ~= nil, "CannotStop Timer Twice")
		self.TickHandler:Disconnect()
		self.TickHandler = nil
	end,	
	Delete = function(self)
		if self.TickHandler ~= nil then
			self.TickHandler:Disconnect()
			self.TickHandler = nil
		end
		self = nil
	end,
	TogglePause = function(self)
		if self.state == play then
			self.TickHandler:Disconnect()
			self.TickHandler = nil
			self.state = paused
		elseif self.state == paused then
			self.TickHandler = TickBus.Connect(self)
			self.state = play 
		end
	end
}
local TimerFunctions_mt = {__index = TimerFunctions}
local Timer = {
	create = function(self,duration,loops,scriptComponent,func,data)
		new_inst = {
			scriptComponent = scriptComponent,
			loops = loops,
			remainingLoops = loops,
			func = func,
			duration = duration,
			data = data,
			timeScale = 1.0,
			OnTick = self.OnTick}
		setmetatable(new_inst, TimerFunctions_mt )
		new_inst:Restart()
		return new_inst 
	end,
	OnTick = function(self,deltaTime,scriptTimePoint)
		self.timeout = self.timeout - (deltaTime * self.timeScale)
		if self.timeout <= 0 then
		self.func(self,self.data)
			if self.remainingLoops == 1 then
				self:Stop()
			else
				if self.remainingLoops > 0 then
				self.remainingLoops = self.remainingLoops - 1
				end
				self.timeout = self.duration
			end
		end
	end
}return Timer