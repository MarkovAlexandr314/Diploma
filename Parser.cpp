#include "Parser.h"
#include <assert.h>
#include <iostream>

const char *COORDS_NAMES[] = {"CoordinateX",
                              "CoordinateY",
                              "CoordinateZ"};

void Parser::ReadCoordinates()
{

    int isize[3][3], ind_min[3], ind_max[3];
    char zonename[50];
    ZoneType_t zone_type;
    DataType_t type;
    char coordname[128];
    coords_by_zone.resize(zone_number);

    for (size_t index_zone = 0; index_zone < zone_number; index_zone++)
    {
        // Получаем тип сетки
        if (cg_zone_type(fn, index_base, index_zone + 1, &zone_type))
            cg_error_exit();
        // Пока обрабатываем только структурированные сетки
        if (zone_type != Structured)
            assert("Structured zone expected!!!");
        // Читаем размеры и имя зоны
        if (cg_zone_read(fn, index_base, index_zone + 1, zonename, isize[0]))
            cg_error_exit();

        // Минимальный индекс в зоне
        ind_min[COORD::X] = 1;
        ind_min[COORD::Y] = 1;
        ind_min[COORD::Z] = 1;

        // Максимальный индекс в зоне
        ind_max[COORD::X] = isize[0][COORD::X];
        ind_max[COORD::Y] = isize[0][COORD::Y];
        ind_max[COORD::Z] = isize[0][COORD::Z];

        // Аллоцируем необходимое место в памяти
        // TODO: Проверить можно ли эффективнее распределять это дело в памяти
        coords_by_zone[index_zone].resize(ind_max[COORD::X]);
        for (size_t i{0}; i < ind_max[COORD::X]; i++)
        {
            coords_by_zone[index_zone][i].resize(ind_max[COORD::Y]);
            for (size_t j{0}; j < ind_max[COORD::Y]; j++)
            {
                coords_by_zone[index_zone][i][j].resize(ind_max[COORD::Z]);
                for (size_t k{0}; k < ind_max[COORD::Z]; k++)
                {
                    coords_by_zone[index_zone][i][j][k].resize(dim_number);
                }
            }
        }

        // Читаем кооринаты сетки
        std::vector<double> temp(ind_max[COORD::X] * ind_max[COORD::Y] * ind_max[COORD::Z]);
        for (size_t curr_dim = 0; curr_dim < dim_number; curr_dim++)
        {
            if (cg_coord_info(fn, index_base, index_zone + 1, curr_dim + 1, &type, coordname))
                cg_error_exit();
            if (cg_coord_read(fn, index_base, index_zone + 1, coordname, type, ind_min, ind_max, &temp[0]))
                cg_error_exit();

            // now map temporary (1D) coordinate array to 3D coordinate array
            for (int vertex = 0; vertex < ind_max[COORD::X] * ind_max[COORD::Y] * ind_max[COORD::Z]; ++vertex)
            {
                unsigned i = vertex % ind_max[COORD::X];
                unsigned j = vertex % ind_max[COORD::Y];
                unsigned k = vertex / (ind_max[COORD::X] * ind_max[COORD::Y]);

                coords_by_zone[index_zone][i][j][k][curr_dim] = temp[vertex];
            }
        }
    }
}

void Parser::ReadInterfaces()
{

    int nzones;
    cg_nzones(fn, 1, &nzones);

    for (int Z = 1; Z <= nzones; Z++)
    {
        int n1to1 = 0, nconns = 0;
        cg_n1to1(fn, 1, Z, &n1to1);
        cg_nconns(fn, 1, Z, &nconns);

        char zonename[30];
        // cg_zone_read(fn, 1, Z, zonename, nullptr);

        std::cout << "Zone " << Z << " (" << zonename << "): "
                  << "n1to1=" << n1to1 << ", nconns=" << nconns << "\n";

        // Если nconns > 0 — смотрим тип каждой обобщённой связи
        for (int C = 1; C <= nconns; C++)
        {
            char connname[30];
            GridConnectivityType_t ctype;
            char donorname[30];
            cg_conn_info(fn, 1, Z, C, connname, nullptr, &ctype,
                         nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
            std::cout << "  conn " << C << ": name=" << connname
                      << ", type=" << ctype // CG_ABUTTING1TO1 = 2?
                      << ", donor=" << donorname << "\n";
        }
    }

    int CGNS_MAX_NAME_LENGTH{40};
    int numberInterfaces{0};
    // Количество интерфейсов, которые хранятся глобально под базовым узлом
    // std::vector<std::vector<char>> interfaceName(numberInterfaces, std::vector<char>(CGNS_MAX_NAME_LENGTH + 1));
    // std::vector<std::vector<char>> zoneName(numberInterfaces, std::vector<char>(CGNS_MAX_NAME_LENGTH + 1));
    // std::vector<std::vector<char>> donorName(numberInterfaces, std::vector<char>(CGNS_MAX_NAME_LENGTH + 1));

    // std::vector<std::vector<cgsize_t>> owner(numberInterfaces);
    // std::vector<std::vector<cgsize_t>> neighbour(numberInterfaces);
    // std::vector<std::vector<cgsize_t>> transform(numberInterfaces);

    // std::vector<char *> interfaceNamePtr(numberInterfaces);
    // std::vector<char *> zoneNamePtr(numberInterfaces);
    // std::vector<char *> donorNamePtr(numberInterfaces);
    // std::vector<cgsize_t *> ownerPtr(numberInterfaces);
    // std::vector<cgsize_t *> neighbourPtr(numberInterfaces);
    // for (int i = 0; i < numberInterfaces; ++i)
    // {
    //     interfaceNamePtr[i] = interfaceName[i].data();
    //     zoneNamePtr[i] = zoneName[i].data();
    //     donorNamePtr[i] = donorName[i].data();
    //     ownerPtr[i] = owner[i].data();
    //     neighbourPtr[i] = neighbour[i].data();
    // }

    // Наши сетки пока хранят только интерфесы каждая для себе
    // TODO: Расширить функционал
    // int n1to1;
    int itranfrm[3];
    int n1to1;
    int index_file, nzone, index_zone, index_conn;
    char donorname[33], connectname[33];
    cgsize_t ipnts[2][3], ipntsdonor[2][3];
    for (size_t index_zone = 0; index_zone < zone_number; index_zone++)
    {
        // Получаем число интерфейсов для данной зоны
        std::cout << index_base << ' ' << index_zone + 1 << ' ' << n1to1 << std::endl;
        if (cg_n1to1(fn, index_base, index_zone + 1, &n1to1))
            cg_error_exit();
        std::cout << index_zone + 1 << ' ' << n1to1 << std::endl;
        interface_connectivity[index_zone].resize(n1to1);
        //=======================

        // int nzones;
        // cg_nzones(fn, 1, &nzones);

        // for (int Z = 1; Z <= nzones; Z++)
        // {
        //     int n1to1 = 0, nconns = 0;
        //     cg_n1to1(fn, 1, Z, &n1to1);
        //     cg_nconns(fn, 1, Z, &nconns);
        //     //=======================
        for (size_t index_conn = 0; index_conn < n1to1; index_conn++)
        {
            // Читаем каждый интерфейс в зоне
            if (cg_1to1_read(fn, index_base, index_zone + 1, index_conn + 1, connectname, donorname,
                             ipnts[0], ipntsdonor[0], itranfrm))
                cg_error_exit();

            // Проверка: записан ли в Transform [0, 0, 0]
            // Если записан меняем [0, 0, 0] -> [1, 2, 3]
            if (!itranfrm[0] && !itranfrm[1] && !itranfrm[2])
            {
                itranfrm[0] = 1;
                itranfrm[1] = 2;
                itranfrm[2] = 3;
            }

            std::vector<std::vector<int>> tMatrix(dim_number, std::vector<int>(dim_number));
            for (int dimension = 0; dimension < dim_number; ++dimension)
            {
                int value = itranfrm[dimension];
                tMatrix[std::abs(value) - 1][dimension] = (value > 0) ? 1 : ((value < 0) ? -1 : 0);
            }

            // Считаем случай: стыковка по некоторой грани(одна из компонент i, j, k = const)
            unsigned numberOfIndicesX = std::abs((int)ipnts[0][COORD::X] - (int)ipnts[1][COORD::X]);
            unsigned numberOfIndicesY = std::abs((int)ipnts[0][COORD::Y] - (int)ipnts[1][COORD::Y]);
            unsigned numberOfIndicesZ = std::abs((int)ipnts[0][COORD::Z] - (int)ipnts[1][COORD::Z]);
            unsigned numberOfIndices = std::max(std::max(numberOfIndicesX + 1, numberOfIndicesY + 1),
                                                numberOfIndicesZ + 1);

            interface_connectivity[index_zone][index_conn].ownerIndex.resize(numberOfIndices);
            interface_connectivity[index_zone][index_conn].donorIndex.resize(numberOfIndices);

            for (unsigned index = 0; index < numberOfIndices; ++index)

            {
                const unsigned ownerStartX = (int)ipnts[0][COORD::X] - 1;
                const unsigned ownerStartY = (int)ipnts[0][COORD::Y] - 1;
                const unsigned ownerStartZ = (int)ipnts[0][COORD::Z] - 1;

                const unsigned neighbourStartX = (int)ipntsdonor[0][COORD::X] - 1;
                const unsigned neighbourStartY = (int)ipntsdonor[0][COORD::Y] - 1;
                const unsigned neighbourStartZ = (int)ipntsdonor[0][COORD::Z] - 1;

                const unsigned indexX = numberOfIndicesX == 0 ? 0 : index;
                const unsigned indexY = numberOfIndicesY == 0 ? 0 : index;
                const unsigned indexZ = numberOfIndicesZ == 0 ? 0 : index;

                unsigned ownerIndexX = ownerStartX + indexX;
                unsigned ownerIndexY = ownerStartY + indexY;
                unsigned ownerIndexZ = ownerStartZ + indexZ;

                // calculate the i, j indices of the neighbour zone with the transformation matrix and owner indices. See
                // https://cgns.github.io/CGNS_docs_current/sids/cnct.html#Transform for more information about this equation
                unsigned neighbourIndexX = tMatrix[0][0] * indexX + tMatrix[0][1] * indexY + tMatrix[0][2] * indexZ + neighbourStartX;
                unsigned neighbourIndexY = tMatrix[1][0] * indexX + tMatrix[1][1] * indexY + tMatrix[1][2] * indexZ + neighbourStartY;
                unsigned neighbourIndexZ = tMatrix[2][0] * indexX + tMatrix[2][1] * indexY + tMatrix[2][2] * indexZ + neighbourStartZ;

                // write connectivty information to interface connectivity structure
                // std::cout << donorname << std::endl;
                interface_connectivity[index_zone][index_conn].donor_zone_name = donorname;
                interface_connectivity[index_zone][index_conn].ownerIndex[index][COORD::X] = ownerIndexX;
                interface_connectivity[index_zone][index_conn].ownerIndex[index][COORD::Y] = ownerIndexY;
                interface_connectivity[index_zone][index_conn].ownerIndex[index][COORD::Z] = ownerIndexZ;

                interface_connectivity[index_zone][index_conn].donorIndex[index][COORD::X] = neighbourIndexX;
                interface_connectivity[index_zone][index_conn].donorIndex[index][COORD::Y] = neighbourIndexY;
                interface_connectivity[index_zone][index_conn].donorIndex[index][COORD::Z] = neighbourIndexZ;
            }
        }
    }
}

void Parser::ReadSolution()
{
    int nsols{0};
    CGNS_ENUMT(GridLocation_t)
    loc;
    char solname[33], zonename[33], coordname[33];
    int isize[3][3], ind_min[3], ind_max[3];
    flow_solution.resize(zone_number);
    DataType_t type;
    for (size_t index_zone = 0; index_zone < zone_number; index_zone++)
    {
        // Получаем, сколько решений
        if (cg_nsols(fn, index_base, index_zone + 1, &nsols))
            cg_error_exit();
        // По
        flow_solution[index_zone].resize(nsols);
        for (size_t index_flow = 0; index_flow < nsols; index_flow++)
        {
            cg_sol_info(fn, index_base, index_zone + 1, index_flow + 1, solname, &loc);
            if (loc != CGNS_ENUMV(CellCenter))
            {
                printf("\nError, GridLocation must be Vertex! Currently: %s\n",
                       GridLocationName[loc]);
                exit(1);
            }
            if (cg_zone_read(fn, index_base, index_zone + 1, zonename, isize[0]))
                cg_error_exit();

            ind_min[COORD::X] = 1;
            ind_min[COORD::Y] = 1;
            ind_min[COORD::Z] = 1;

            // Максимальный индекс в зоне
            ind_max[COORD::X] = isize[1][COORD::X];
            ind_max[COORD::Y] = isize[1][COORD::Y];
            ind_max[COORD::Z] = isize[1][COORD::Z];
            flow_solution[index_zone][index_flow].resize(ind_max[COORD::X]); // ресайз для X
            for (size_t i{0}; i < ind_max[COORD::X]; i++)
            {
                flow_solution[index_zone][index_flow][i].resize(ind_max[COORD::Y]);
                for (size_t j{0}; j < ind_max[COORD::Y]; j++)
                {
                    flow_solution[index_zone][index_flow][i][j].resize(ind_max[COORD::Z]);
                    for (size_t k{0}; k < ind_max[COORD::Z]; k++)
                    {
                        flow_solution[index_zone][index_flow][i][j][k].resize(dim_number);
                    }
                }
            }
            // Читаем решение потока
            std::vector<double> temp(ind_max[COORD::X] * ind_max[COORD::Y] * ind_max[COORD::Z]);
            for (size_t curr_dim = 0; curr_dim < dim_number; curr_dim++)
            {
                if (cg_coord_info(fn, index_base, index_zone + 1, curr_dim + 1, &type, coordname))
                    cg_error_exit();
                if (cg_field_read(fn, index_base, index_zone + 1, index_flow + 1, "Density",
                                  type, ind_min, ind_max, temp.data()))
                    cg_error_exit();

                // now map temporary (1D) coordinate array to 3D coordinate array
                for (int vertex = 0; vertex < ind_max[COORD::X] * ind_max[COORD::Y] * ind_max[COORD::Z]; ++vertex)
                {
                    unsigned i = vertex % ind_max[COORD::X];
                    unsigned j = vertex % ind_max[COORD::Y];
                    unsigned k = vertex / (ind_max[COORD::X] * ind_max[COORD::Y]);

                    flow_solution[index_zone][index_flow][i][j][k][curr_dim] = temp[vertex];
                }
            }
            // cg_field_read(fn, index_base, index_zone, index_flow, "Density",
            //               CGNS_ENUMV(RealSingle), irmin, irmax, r[0][0]);
        }
    }
}

const FlowSolution3D_Type Parser::GetFlowSolution()
{
    return flow_solution;
}

const int Parser::GetZoneNumber()
{
    return zone_number;
}

// Получить интерфейс связностей
const InterfaceConnectivity_Type Parser::GetInterfaceConnectivity()
{
    return interface_connectivity;
}

const std::unordered_map<std::string, int> Parser::GetZoneNameMap()
{
    return zone_name_map;
}

Parser::Parser(const std::string &filename_)
{
    filename = filename_;
    char zonename[50];
    int isize[3][3];

    // Открываем файл на чтение
    if (cg_open(filename.c_str(), CG_MODE_READ, &fn))
        cg_error_exit();

    // Получаем кол-во зон
    if (cg_nzones(fn, index_base, &zone_number))
        cg_error_exit();

    // Сопоставляем пары: имя зоны -> индекс
    for (size_t index_zone = 0; index_zone < zone_number; index_zone++)
    {
        if (cg_zone_read(fn, index_base, index_zone + 1, zonename, isize[0]))
            cg_error_exit();

        zone_name_map[zonename] = index_zone + 1;
    }
}

Parser::~Parser()
{
    cg_close(fn);
}
