#ifndef PARSER_H
#define PARSER_H
#include <unordered_map>
#include <vector>
#include <string>
#include "cgnslib.h"

enum COORD
{
    X = 0,
    Y,
    Z
};

template <typename IndexType>
struct InterfaceConnectivity
{
    std::string donor_zone_name;
    std::vector<IndexType> ownerIndex;
    std::vector<IndexType> donorIndex;
};

using Vector3D_Type = std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>>;
using ZoneMap_Type = std::unordered_map<std::string, int>;
using Index_Type = std::array<unsigned, 3>;
using InterfaceConnectivity_Type = std::unordered_map<int, std::vector<InterfaceConnectivity<Index_Type>>>;
using FlowSolution3D_Type = std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>>>;
using FlowSolutionMap_Type = std::vector<std::vector<std::vector<std::string>>>;

class Parser
{
private:
    std::string filename;
    int fn;
    int index_base{1}; // Всегда одна база!!!
    int zone_number{0};
    int dim_number{3}; // Работаем с трехмерными сетками!!!
    Vector3D_Type coords_by_zone;
    ZoneMap_Type zone_name_map;
    InterfaceConnectivity_Type interface_connectivity;
    FlowSolution3D_Type flow_solution;
    FlowSolutionMap_Type flow_solution_map;

public:
    Parser(const std::string &);
    // Читаем координаты
    void ReadCoordinates();
    // Читаем интерфейсы
    void ReadInterfaces();
    // Читаем решение
    void ReadSolution();

    // Геттеры
    // Получить номер зоны
    int GetZoneNumber() const;
    // Получить координаты
    Vector3D_Type GetCoordinates() const;
    // Получить map зон: имя - индекс
    ZoneMap_Type GetZoneNameMap() const;
    // Получить интерфейс соединений
    InterfaceConnectivity_Type GetInterfaceConnectivity() const;
    // Получить решение потока
    FlowSolution3D_Type GetFlowSolution() const;
    // Получить map: index_zone -> flow_number
    FlowSolutionMap_Type GetFlowSolMap() const;

    ~Parser();
};

#endif // PARSER_H