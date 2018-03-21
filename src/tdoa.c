#include "tdoa.h"
#include "math.h"

/*var*/
aPOINT mic_loc[MIC]={
        {0, 70.7, 0},           // mm
        {70.7, 0, 0},
        {0, -70.7, 0},
        {-70.7, 0, 0},
};
aPOINT mic_array_origin = {0,0,31.9};


/**********************
* 创建tdoa表格
* 使用的是最简单的遍历法
**********************/
void create_tdoa_table(int8_t TDOA_table[MIC_PAIR][SEARCH_POINT])
#ifdef USE_CARTESIAN_COORDINATE
{
    float d1,d2,dd;
    // for accuratee calculation
    aPOINT tmp_apoint;
    cPOINT tmp_cpoint;
    int8_t tdoa;
    int8_t mic_pair_cnt = 0;
    for(int i=0; i<SEARCH_POINT; i++){
       // printf("x is %f y is %f z is %f\n",search_cart_coords[i][0],search_cart_coords[i][1],search_cart_coords[i][2]);
        mic_pair_cnt = 0;
        tmp_cpoint = index2xyz(i);
        tmp_apoint.x = tmp_cpoint.x;
        tmp_apoint.y = tmp_cpoint.y;
        tmp_apoint.z = tmp_cpoint.z;
        for(int8_t j=0; j<MIC; j++){
            for(int8_t k=j+1; k<MIC; k++){
                d1 = calulate_distance(tmp_apoint, mic_loc[k]);
                d2 = calulate_distance(tmp_apoint, mic_loc[j]);
                dd = d2 - d1;
                tdoa = round((dd/C)*FS);
//                printf("%d, ", tdoa);
                TDOA_table[mic_pair_cnt][i] = tdoa;
                mic_pair_cnt++;
            }
        }
//        printf("\n");
    }
}
#else
{
    float d1,d2,dd;
    // for accuratee calculation
    aPOINT tmp_apoint;
    cPOINT tmp_cpoint;
    int8_t tdoa;
    int8_t mic_pair = 0;
	for(int i=0; i<SEARCH_POINT; i++){
       // printf("x is %f y is %f z is %f\n",search_cart_coords[i][0],search_cart_coords[i][1],search_cart_coords[i][2]);
	    mic_pair = 0;
	    for(int8_t j=0; j<MIC; j++){
			for(int8_t k=j+1; k<MIC; k++){
			    tmp_cpoint = sph_to_cart(index2tpr(i));
			    tmp_apoint.x = tmp_cpoint.x;
			    tmp_apoint.y = tmp_cpoint.y;
			    tmp_apoint.z = tmp_cpoint.z;
				d1 = calulate_distance(tmp_apoint, mic_loc[k]);
				d2 = calulate_distance(tmp_apoint, mic_loc[j]);
				dd = d2 - d1;
               // printf("tdooa is %f\n",tdooa);
                tdoa = round((dd/C)*FS);
//                printf("int tdoa is %d\n",tdoa);
				TDOA_table[mic_pair][i]=tdoa;
				mic_pair++;
			}
        }
	}
}
#endif

/**********************
* 将球坐标转换成直角坐标
**********************/
cPOINT sph_to_cart(pPOINT point)
{
    cPOINT cpoint;
    cpoint.x = point.r*cos(point.phi)*cos(point.theta) + mic_array_origin.x;
    cpoint.y = point.r*cos(point.phi)*sin(point.theta) + mic_array_origin.y;
    cpoint.z = point.r*sin(point.phi) + mic_array_origin.z;
    return cpoint;
  //  printf("z  theta is %f %f\n",sph[1],plot[2]);
}

/**********************
* 将直角坐标转换成球坐标
**********************/
pPOINT cart_to_sph(cPOINT point)
{
    pPOINT ppoint;
    ppoint.theta = atan2(point.y, point.x) * 180 /PI;
    float xy = sqrt((double)(point.x*point.x + point.y*point.y));
    ppoint.phi = atan2(point.z, xy) * 180 /PI;
    ppoint.r = (int)sqrt((double)(point.x*point.x + point.y*point.y + point.z*point.z));
//    printf("xyz:%d, %d, %d, tpr: %d, %d, %d\n", point.x, point.y, point.z,
//            ppoint.theta, ppoint.phi, ppoint.r);
    return ppoint;
}

/**********************
* 计算两个坐标之间的距离
* 输入：两个点的极坐标
* 输出：两点之间的距离
**********************/
float calulate_distance(aPOINT x1 ,aPOINT x2)
{
    float distance;
    distance = sqrt( pow(x1.x-x2.x,2) + pow(x1.y-x2.y,2) + pow(x1.z-x2.z,2) );
    return distance;
}

#ifdef USE_CARTESIAN_COORDINATE
cPOINT index2xyz(int16_t index)
{
//    % index to xyz
//    %  (z - 1) * SEARCH_X_NUM * SEARCH_Y_NUM
//    % +(y+(SEARCH_X_NUM/2) -1) * SEARCH_X_NUM
//    % +(x+(SEARCH_X_NUM/2) -1) + 1 = index
//    % cart = [(x-31)*100, (y-31)*100, (z-1)*300]
    cPOINT point;
    point.x = ((index % SEARCH_X_NUM) - (SEARCH_X_NUM/2)) * SEARCH_X_STEP;
    point.y = (((index / SEARCH_X_NUM) % SEARCH_Y_NUM) - (SEARCH_X_NUM/2)) * SEARCH_Y_STEP;
    point.z = ((index / SEARCH_X_NUM) / SEARCH_Y_NUM) * SEARCH_Z_STEP;
//    printf("index: %d, x: %d, y: %d, z:%d\n", index, point.x, point.y, point.z);
    return point;
//    % cart = [(x-31)*100, (y-31)*100, (z-1)*300]
}

//int16_t xyz2index(cPOINT point)
//{
////    % xyz to index
////    %  (z-1)*size(search_range.x, 2)*size(search_range.y, 2)
////    % +(y-1)*size(search_range.x, 2)
////    % +(x-1) + 1 = index
//    int16_t index = {0};
////    index = (z-1)*61*61 + (y-1)*61 + (x-1)+1;
//    return index;
////    % cart = [(x-31)*100, (y-31)*100, (z-1)*300]
//}

#else
pPOINT index2tpr(int16_t index)
{
//    % index to xyz
//    %  (R-1) * SEARCH_PHI_NUM * SEARCH_THETA_NUM
//    % +(PHI-1) * SEARCH_THETA_NUM
//    % +(THETA - 1) + 1 = index
//    % cart = [(x-31)*100, (y-31)*100, (z-1)*300]
    pPOINT point = {0};
    point.theta = (((index-1) % SEARCH_THETA_NUM) + 1) * SEARCH_THETA_STEP;
    point.phi = (((index / SEARCH_THETA_NUM) % SEARCH_PHI_NUM) + 1) * SEARCH_PHI_STEP;
    point.r = (((index / SEARCH_THETA_NUM) / SEARCH_PHI_NUM) + 1) * SEARCH_R_STEP;
    return point;
//    % cart = [(x-31)*100, (y-31)*100, (z-1)*300]
}

int16_t tpr2index(pPOINT point)
{
//    % xyz to index
//    %  (z-1)*size(search_range.x, 2)*size(search_range.y, 2)
//    % +(y-1)*size(search_range.x, 2)
//    % +(x-1) + 1 = index
    int16_t index = {0};
//    index = (z-1)*61*61 + (y-1)*61 + (x-1)+1;
    return index;
//    % cart = [(x-31)*100, (y-31)*100, (z-1)*300]
}
#endif

