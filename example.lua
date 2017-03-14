

function render(pBuffer)
   local buffer = HSVBuffer(pBuffer)
   local size = buffer:size()

   for i = 0, size -1 do
       buffer:set(i, i * (360.0/size), 0.8, 0.8)
   end
end