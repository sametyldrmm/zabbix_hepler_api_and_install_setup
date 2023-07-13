#!/bin/bash

./run_program/get_items  method:item.get limit:5 with_triggers:true search:key_:system.cpu output:key_,name,lastvalue itemids:22842,22882,22922,22962,23002
./run_program/get_items  method:trigger.get output:triggerid,description limit:5 output:event_name,description,value
