// MatrixEquity.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "ProbToWinWithCard.h"
#include "CardsCombos.h"

#include "MatrixStatus.h"
#include "FixedMatrixLayers.h"
#include "EqSummTables.h"
#include "setsOfHittedCells.h"

int main()
{
    cout << __cplusplus << endl;
    std::map<std::string, float> testParseFileResult;



    try{
        //read EV and probability data;
        ProbToWinWithCard* PTWWC = new ProbToWinWithCard();

        //fill cards and combos arrays;
        vector<string> combos;
        for (auto el : PTWWC->combosEvProbGlobal) {
            combos.push_back(el.first);
        }
        CardsCombos* CardCombos = new CardsCombos(combos);
        EqSummTables* eqSummTable = new EqSummTables(PTWWC, CardCombos);
        setsOfHittedCells* setHittedCells = new setsOfHittedCells;
        setHittedCells->formForAllMatrixStatuses();


        vector<string> testMatrix = { "Ks", "2d","9c","7s",
                                        "Jc", "Qc", "As", "Ad",
                                        "Qd", "7d", "8d", "5d",
                                        "3s", "4c", "4s", "3c" };
        FixedMatrixLayers FML(testMatrix, PTWWC, CardCombos, eqSummTable, setHittedCells);
        FLayer* OldLayer = new FLayer;
        FML.FillStartLayer(OldLayer);
        cout << "start layer is ready" << endl;
        FLayer* NextOne = new FLayer;
        for (int i = 0; i < 50; ++i) {
            FML.FillNextLayer(OldLayer, NextOne);
            swap(OldLayer, NextOne);
            cout << "layer " << i << "calculated" << endl;
        }


        /*cout << "saving";
        ML.SaveLayerToHdd(100000, layer);
        Layer nextOne;
        for (int i = 0; i < 3; ++i){
            nextOne = ML.NextLayer(layer);
            ML.SaveLayerToHdd(100000, layer);
            layer = nextOne;
        }*/


        cout << "reading data ok" << endl;
    }
    catch (const std::runtime_error& e){
        std::cout << e.what();
        std::cout << "GOT ERROR" << std::endl;
        
    }
    std::cout << "finish" << std::endl;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
