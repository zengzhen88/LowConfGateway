#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int32_t main(int32_t args, char *argv[]) {
#ifdef UNITPRICE
    float CornRadio = 0.0;  /*玉米占比*/
    float SoybeanMealRadio = 0.0; /*豆粕占比*/
    float WheatBranRadio = 0.0; /*麦麸占比*/
    float SaltRadio = 0.0; /*盐占比*/
    float CalciumHydrogenPhosphateRadio = 0.0; /*磷酸氢钙占比*/
    float ShellPowderRadio = 0.0; /*贝壳粉占比*/
    float PreMixedMaterialRadio = 0.0; /*预混料占比*/

    float CornUnitPrice = 0.0;  /*玉米单价*/
    float SoybeanMealUnitPrice = 0.0; /*豆粕单价*/
    float WheatBranUnitPrice = 0.0; /*麦麸单价*/
    float SaltUnitPrice = 0.0; /*盐单价*/
    float CalciumHydrogenPhosphateUnitPrice = 0.0; /*磷酸氢钙单价*/
    float ShellPowderUnitPrice = 0.0; /*贝壳粉单价*/
    float PreMixedMaterialUnitPrice = 0.0; /*预混料单价*/

#define Corn (1.25) /*玉米*/
#define SoybeanMeal (1.7) /*豆粕*/
#define WheatBran (1.07) /*麦麸*/
#define Salt (0.3) /*盐*/
#define CalciumHydrogenPhosphate (2.3) /*磷酸氢钙*/
#define ShellPowder (0) /*贝壳粉*/
#define PreMixedMaterial_0_3_zhongmeipuke (7.35) /*预混料*/

    /*中美普克*/
    CornRadio = atof(argv[1]);
    CornUnitPrice = atof(argv[2]);
    printf ("玉米占比:%f 单价:%f\n", CornRadio, CornUnitPrice);

    SoybeanMealRadio = atof(argv[3]);
    SoybeanMealUnitPrice = atof(argv[4]);//SoybeanMeal;
    printf ("豆粕占比:%f 单价:%f\n", SoybeanMealRadio, SoybeanMealUnitPrice);

    WheatBranRadio = atof(argv[5]);//0.052;
    WheatBranUnitPrice = atof(argv[6]);//WheatBran;
    printf ("麦麸占比:%f 单价:%f\n", CornRadio, CornUnitPrice);

    SaltRadio = atof(argv[7]);//0.006;
    SaltUnitPrice = atof(argv[8]);//Salt;
    printf ("盐占比:%f 单价:%f\n", SaltRadio, SaltUnitPrice);

    CalciumHydrogenPhosphateRadio = atof(argv[9]);//0.015;
    CalciumHydrogenPhosphateUnitPrice = atof(argv[10]);//Corn;
    printf ("磷酸氢钙占比:%f 单价:%f\n", CalciumHydrogenPhosphateRadio, CalciumHydrogenPhosphateUnitPrice);

    ShellPowderRadio = atof(argv[11]);//0.017;
    ShellPowderUnitPrice = atof(argv[12]);//ShellPowder;
    printf ("贝壳粉占比:%f 单价:%f\n", ShellPowderRadio, ShellPowderUnitPrice);

    PreMixedMaterialRadio = atof(argv[13]);//0.02;
    PreMixedMaterialUnitPrice = atof(argv[14]);//PreMixedMaterial;
    printf ("预混料占比:%f 单价:%f\n", PreMixedMaterialRadio, PreMixedMaterialUnitPrice);

    float radio = CornRadio + SoybeanMealRadio + WheatBranRadio + SaltRadio + CalciumHydrogenPhosphateRadio + ShellPowderRadio + PreMixedMaterialRadio;
    printf ("radio all:%f\n", radio);
    
    float unitPrice = CornRadio * CornUnitPrice 
        + SoybeanMealRadio * SoybeanMealUnitPrice 
        + WheatBranRadio * WheatBranUnitPrice 
        + SaltRadio * SaltUnitPrice 
        + CalciumHydrogenPhosphateRadio * CalciumHydrogenPhosphateUnitPrice 
        + ShellPowderRadio * ShellPowderUnitPrice 
        + PreMixedMaterialRadio * PreMixedMaterialUnitPrice;
    printf ("每斤精料单价:%f\n", unitPrice);
#endif

#ifdef ALLPRICE
    float NurturingPeriod_0_3 = 45.15; /*育雏期0-3月需求精料重量斤*/
    float NurturingPeriod_3_6 = 130.2; /*育雏期3-6月需求精料重量斤*/
    float IncubationPeriod_7_8 = 142.8; /*育成期7-8月需求精料重量斤*/
    float IncubationPeriod_9_up = 210; /*育成期9_10.5月需求精料重量斤*/

    float all = NurturingPeriod_0_3 * atof(argv[1]) 
        + NurturingPeriod_3_6 * atof(argv[2]) 
        + IncubationPeriod_7_8 * atof(argv[3])
        + IncubationPeriod_9_up * atof(argv[4]);
    printf ("一只商品鸟养殖到10.5个月需要成本:%f\n", all);
#endif
    

    return 0;
}
