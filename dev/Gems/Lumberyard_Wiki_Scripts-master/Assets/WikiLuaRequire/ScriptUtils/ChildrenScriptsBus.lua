--ChildrenScriptsBus

-- Description:

-- it makes possible to access another script component table in a parent script setting it to a variable or adding it to an array.
-- it creates a new array if no one exists with that name in the parent component script.

-- How to use:
-- Require the script in your parent and children script components using  local ChildrenScriptsBus = require "WikiLuaRequire/ScriptUtils/ChildrenScriptsBus" 

-- in the parent script component:
-- in onActivate  call self.ChildrenScriptsHandler = ChildrenScriptsBus:Connect(self,false) 
-- then disconnect with self.ChildrenScriptsHandler:Disconnect() or call only ChildrenScriptsBus:Connect(self,true) in OnActivate ( it will disconnect automatically the next frame(actually 5 but I don't remember why I set it this way).

-- in the children script component:
-- in onActivate call ChildrenScriptsBus.Event:Add(parentEntityId,self,"arrayName") or ChildrenScriptsBus.Event:Set(parentEntityId,self,"varName")

-- if change the name of the require local variable you need to change all your api calls.
_G.TempTable = 0
local addCrc = Crc32("add").value
local setCrc = Crc32("set").value -- required if you add another event
local ChildrenScriptsBus = {
	Connect = function(self,parentArg,AutoDisconnect)
		local Handler = {
			frame = 0,
			parent = parentArg,
			OnTick = self.Private.OnTick,
			Disconnect = self.HandlerOnly.Disconnect,
			OnEventBegin = self.Private.OnEventBegin,
		}
		Handler.addHandler = GameplayNotificationBus.Connect(Handler,GameplayNotificationId(parentArg.entityId,"add"))
		Handler.setHandler = GameplayNotificationBus.Connect(Handler,GameplayNotificationId(parentArg.entityId,"set"))
		if AutoDisconnect == true then
			Handler.tickHandler = TickBus.Connect(Handler)
			Handler.AutoDisconnect = self.Private.AutoDisconnect
			return nil
		end
		return Handler
	end,
	Private = {
		OnTick = function(self, deltaTime, timePoint)
			self.frame = self.frame + 1
			if(self.frame == 5 ) then
				self:AutoDisconnect()
			end
		end,
		AutoDisconnect = function(self)
			self.tickHandler:Disconnect()
			self:Disconnect()
		end,
		OnEventBegin = function(self,value)
			local storage = self.parent[value]
			local instance = _G.TempTable
			local busId = GameplayNotificationBus.GetCurrentBusId().actionNameCrc.value
			if(busId == addCrc)then
				if(storage == nil)then
					self.parent[value] = {}
					storage = self.parent[value]
				end
				storage[#storage + 1] = instance
			--else if(busId == setCrc)then -- required if you add another event.
			else
				self.parent[value] = instance
			end
			instance.parent = self.parent
			_G.TempTable = 0
		end,
	},
	HandlerOnly = {
		Disconnect = function(self)
			self.addHandler:Disconnect()
			self.setHandler:Disconnect()
			self = nil --destroyes handler.
		end,
	},
	Event = {
		Add = function (self,ParentIdArg,componentArg,keynameArg)
			self.Private:Setup(componentArg,ParentIdArg,"add",keynameArg)
		end,
		Set = function (self,ParentIdArg,componentArg,keynameArg)
			self.Private:Setup(componentArg,ParentIdArg,"set",keynameArg)
		end,
		Private = {
			Setup = function (self,componentArg,ParentIdArg,verbArg,keynameArg)
				local new = {
					component = componentArg,
					parentId = ParentIdArg,
					verb = verbArg,
					keyname = keynameArg,
					OnTick = self.OnTick,
					EndInit = self.EndInit,
				}
				new.tickBusHandler = TickBus.Connect(new)
				if(componentArg.childScriptRefs == nil)then
					componentArg.childScriptRefs = {}
				end
				local key = tostring(ParentIdArg)
				componentArg.childScriptRefs[key] = new
			end,
			OnTick = function(self,deltaTime, timePoint)
				_G.TempTable = self.component
				local gid = GameplayNotificationId(self.parentId,self.verb)
				GameplayNotificationBus.Event.OnEventBegin(gid,self.keyname)
				self:EndInit()
			end,
			EndInit = function(self)
				self.tickBusHandler:Disconnect()
				self.component.childScriptRefs[tostring(self.parentId)] = nil
				self = nil
			end,
		},
	},
}
return ChildrenScriptsBus