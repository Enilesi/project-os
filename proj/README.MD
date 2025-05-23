To run treasure_manager.c:

gcc treasure_manager.c -o treasure_manager
./treasure_manager



To run treasure_hub.c:

gcc treasure_hub.c -o treasure_hub
gcc monitor.c -o monitor
gcc -o p.exe calculate_score.c
./treasure_hub


For both programs I added a help command, that prints the commands list provided by one of the programs.

To add a hunt, --add and the hunt's name is sufficient. But for adding a treasure, it must be written:
"--add [hunt_id] treasure". 

To add multiple treasures the command "--add -r [hunt_id]" treasure can be used (the 'r' is standing for repetead).