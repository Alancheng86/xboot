---
-- The 'DisplayNinePatch' class is used to display nine patch related objects that can
-- be placed on the screen.
--
-- @module DisplayNinePatch
local M = Class(DisplayObject)

---
-- Creates a new object of display nine patch.
--
-- @function [parent=#DisplayNinePatch] new
-- @param ninepatch (Table) The table of textures
-- @param width (number) The width of drawing area in pixels.
-- @param height (number) The height of drawing area in pixels.
-- @param x (optional) The x coordinate of the display nine patch.
-- @param y (optional) The y coordinate of the display nine patch.
-- @return #DisplayNinePatch
function M:init(ninepatch, width, height, x, y)
	self.super:init()
	
	assert(type(ninepatch) == "table")
	self.lt = DisplayImage.new(assert(ninepatch.lt))
	self.mt = DisplayImage.new(assert(ninepatch.mt))
	self.rt = DisplayImage.new(assert(ninepatch.rt))
	self.lm = DisplayImage.new(assert(ninepatch.lm))
	self.mm = DisplayImage.new(assert(ninepatch.mm))
	self.rm = DisplayImage.new(assert(ninepatch.rm))
	self.lb = DisplayImage.new(assert(ninepatch.lb))
	self.mb = DisplayImage.new(assert(ninepatch.mb))
	self.rb = DisplayImage.new(assert(ninepatch.rb))

	self.left, self.top = self.lt:getInnerSize()
	self.right, self.bottom = self.rb:getInnerSize()
	
	local mw, mh = self.mm:getInnerSize()
	self.width = self.left + self.right + mw
	self.height = self.top + self.bottom + mh
	
	self:addChild(self.lt)
	self:addChild(self.mt)
	self:addChild(self.rt)
	self:addChild(self.lm)
	self:addChild(self.mm)
	self:addChild(self.rm)
	self:addChild(self.lb)
	self:addChild(self.mb)
	self:addChild(self.rb)
	
	self:setContentSize(width, height)
	self:setPosition(x or 0, y or 0)
end

---
-- Rechange the width and height in pixels.
--
-- @function [parent=#DisplayObject] setContentSize
-- @param self
-- @param width (number) The new width.
-- @param height (number) The new height.
function M:setContentSize(width, height)
	local width = width or self.width
	local height = height or self.height
	local w = width - self.left - self.right
	local h = height - self.top - self.bottom
	
	self.lt:setPosition(0, 0)
	self.lt:setContentSize(self.left, self.top)
	self.mt:setPosition(self.left, 0)
	self.mt:setContentSize(w, self.top)
	self.rt:setPosition(self.left + w, 0)
	self.rt:setContentSize(self.right, self.top)
	
	self.lm:setPosition(0, self.top)
	self.lm:setContentSize(self.left, h)
	self.mm:setPosition(self.left, self.top)
	self.mm:setContentSize(w, h)
	self.rm:setPosition(self.left + w, self.top)
	self.rm:setContentSize(self.right, h)
	
	self.lb:setPosition(0, self.top + h)
	self.lb:setContentSize(self.left, self.bottom)
	self.mb:setPosition(self.left, self.top + h)
	self.mb:setContentSize(w, self.bottom)
	self.rb:setPosition(self.left + w, self.top + h)
	self.rb:setContentSize(self.right, self.bottom)

	return self
end

return M