simba_sbe_protocol=Proto("Simba", "Simba Moscow Protocol")
sequence_number = ProtoField.int32("Simba.sequence_number","sequence_number", base.DEC)
size= ProtoField.uint16("Simba.size", "size",base.DEC)
flags = ProtoField.uint16("Simba.flags","flags", base.DEC)
sending_time = ProtoField.int64("Simba.sendingTime", "sendingTime",base.DEC)
message_id = ProtoField.uint16("Simba.message_id","message_id", base.DEC)

simba_sbe_protocol.fields = {sequence_number,size,flags,sending_time, message_id}
function simba_sbe_protocol.dissector(buffer, pinfo, tree)
    length = buffer:len()
    if length == 0 then return end
    pinfo.cols.protocol=simba_sbe_protocol.name

    local subtree = tree:add(simba_sbe_protocol, buffer(), "Simba Moscow Data")
    subtree:add_le(sequence_number ,buffer(0,4))
    subtree:add_le(size ,buffer(4,2))
    subtree:add_le(flags,buffer(6,2))
    subtree:add_le(sending_time,buffer(8,8))
    f = buffer(6,2):int()
    if f > 8 then 
    subtree:add_le(message_id,buffer(30,2)) -- only for incremental feed
    else 
    subtree:add_le(message_id,buffer(18,2)) -- only for snapshot feed
    end

end
local udp_port = DissectorTable.get("udp.port")
udp_port:add(20082, simba_sbe_protocol)
--udp_port:add(20082, simba_sbe_protocol) 
-- snapshot feed for 20082
-- incremental feed 20081
