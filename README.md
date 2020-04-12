# Placer and Router Software using C++


Code to implement the function of Placement and routing of standard cells using C++.


                                        Placer

Placer uses quadratic wirelength model to minimize the total wirelength of placing gates in the grid.

Placer uses recursive partitioning to separate gates into separate regions where 8x8 produces 64 regions.

Placer consists of 3 steps quadratic placement, assignment and containment.

1. Quadratic Placement gives the positions as output.
2. Assignment distributes the gates into left right or up down  for partitioning 
3. Containment creates virtual pads to contain the gates with the region during quadratic placement.

The placer program (placer.cpp) takes the number of gates,number of nets, number of pads and their positions are given as input and assumes the size of grid (100,100).



//*
3 3                  ->Number of gates and nets
1 2 1 2              ->Gate number, Number of nets connected to gate, Net number
2 2 2 3
3 1 3
2                   ->Number of pads
1 1 0 100           ->Pad number, Net connected to net, Pad_x_pos, Pad_y_pos
2 3 100 0
*//

USE BELOW LINK For Visualization of Placer output (Use one of the outputs present in placer_outputs folder)

https://spark-public.s3.amazonaws.com/vlsicad/javascript_tools/visualize.html


                                        Router
                       
 Router works using Maze Routing Algorithm which uses min heap with cost as key for expansion fron source till target.

The router is capable of routing 2 point nets over 2 layers with a given bend and via penalty with non uniform cost on each grid to avoid congestion

Input needs .grid and .nl netlist file where grid gives the config info about penalty and grid size and netlist contains the number of nets and source, target pairs for each netid. 

The router.cpp gives the routed path as OUTPUT File for each net in the for
/// 1        -> NET NUMBER 1
/// 1 4 8    -> SOURCE
/// 1 4 7
//  1 4 6    -> TARGET
// 0         -> END



USE BELOW LINK For Visualization of Routed output (Use one of the outputs present in outputs folder)

https://spark-public.s3.amazonaws.com/vlsicad/javascript_tools/router.html
