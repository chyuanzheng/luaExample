local emark = ...
local mylib = require "mark"
local cmd = mylib.newcmd(emark)
cmd:addcmd(0x0102)

function DoCMDData (cc)
  local id0,id1,param = string.unpack("BBI10", cc)
  if (id0 == 0x01 and id1 == 0x02) then
    id1 = 0x05
    local ret = string.pack("BBI10", id0, id1, param)
    return ret
  end
  return cc
end

-- print(mylib)
-- print("fdsaf ")
-- local x= mylib.dir("ccc")
-- for k, v in ipairs(x) 
--   do 
--   print(k, v) 
--   end


