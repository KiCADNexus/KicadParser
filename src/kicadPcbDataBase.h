#ifndef KICAD_PCB_KICADPCB_DATABASE_H
#define KICAD_PCB_KICADPCB_DATABASE_H

#include "net.h"
#include "kicadParser.h"
#include "tree.h"
#include "util.h"
#include "module.h"
#include "pin.h"
#include "segment.h"
#include "via.h"
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <assert.h>
#include <iterator>
#include <unordered_map>

class kicadPcbDataBase
{
public:
    kicadPcbDataBase(std::string fileName) : m_fileName(fileName)
    {
        std::cerr << "Build Kicad Pcb database..." << std::endl;
        if (!buildKicadPcb())
        {
            std::cerr << "ERROR: Building Kicad Pcb database failed." << std::endl;
            assert(false);
        }
    };

    ~kicadPcbDataBase(){};

    void printLayer();
    void printNet();
    void printInst();
    void printComp();
    void printNetclass();
    void printPcbRouterInfo();
    void printFile();
    void printSegment();

    bool buildKicadPcb();

    bool getPcbRouterInfo(std::vector<std::set<std::pair<double, double>>> *);
    bool getPinPosition(const std::string &inst_name, const std::string &pin_name, point_2d *pos);
    bool getPinPosition(const int inst_id, const int &pin_id, point_2d *pos);
    void getPinPosition(const padstack &, const instance &, point_2d *pos);
    bool getPinPosition(const pin &p, point_2d *pos);
    bool getInstBBox(const int inst_id, point_2d *bBox);
    // TODO:: Move this to instance or overloaded this to Instance
    void getPadstackRotatedWidthAndHeight(const instance &inst, const padstack &pad, double &width, double &height);

    bool getInstance(const std::string &, instance *&);
    bool getComponent(const std::string &, component *&);
    bool getNet(const std::string &, net *&);

    component &getComponent(const int id) { return components.at(id); }
    instance &getInstance(const int id) { return instances.at(id); }
    net &getNet(const int id) { return nets.at(id); }
    netclass &getNetclass(const int id) { return netclasses.at(id); }

    std::string getFileName() { return m_fileName; }
    std::vector<instance> &getInstances() { return instances; }
    std::vector<component> &getComponents() { return components; }
    std::vector<net> &getNets() { return nets; }

    bool isInstanceId(const int id) { return id < instances.size() ? true : false; }
    bool isComponentId(const int id) { return id < components.size() ? true : false; }
    bool isNetId(const int id) { return id < nets.size() ? true : false; }
    bool isNetclassId(const int id) { return id < netclasses.size() ? true : false; }

    // TODO: All layers are copper in the "layer_to_index_map" and "index_to_layer_map"
    int getNumCopperLayers() { return index_to_layer_map.size(); }
    int getLayerId(const std::string &layerName);
    std::map<int, std::string> &getCopperLayers() { return index_to_layer_map; }
    bool isCopperLayer(const int);
    bool isCopperLayer(const std::string &);

    // TODO: Get design boundary based on rotated pin shape
    void getBoardBoundaryByPinLocation(double &minX, double &maxX, double &minY, double &maxY);

private:
    net &getNet(const std::string &);

private:
    // Input
    std::string m_fileName;

    // Index map
    std::unordered_map<std::string, int> layer_to_index_map;   //<layer name, layer id>
    std::map<int, std::string> index_to_layer_map;             //<layer id, layer name>
    std::unordered_map<std::string, int> net_name_to_id;       //<net name, net id>
    std::unordered_map<int, std::string> net_id_to_name;       //<net id, net name>
    std::unordered_map<std::string, int> instance_name_to_id;  //<instance name, instance int>
    std::unordered_map<std::string, int> component_name_to_id; //<component name, component int>

    // Object Instances
    std::vector<instance> instances;
    std::vector<component> components;
    std::vector<net> nets;
    std::vector<netclass> netclasses;

    // Keepouts
    std::map<std::string, paths> layer_to_keepout_map; // keepout zones <layer name, polygon>
    paths all_keepouts;                                // All keepout zones in polygon

    // TODO: Move to Net Instance and Consider the usage of DRC checking
    std::map<int, paths> net_to_segments_map;

    // TODO: Refactor this
    // Reorganized for router
    std::vector<pad> all_pads; // unconnected pins
    std::vector<track> the_tracks;

    // For differential pair lookup
    std::map<std::string, std::pair<int, int>> name_to_diff_pair_net_map; // <net name, <netId1, netId2>>

    // Misc.
    tree tree;
};

#endif
