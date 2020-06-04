#include "lookup2d.h"

Lookup2D::Lookup2D( const double tableData_in[][2], unsigned int numEntries)
{
    tableStartPointer = &tableData_in[0][0];
    numTableEntries = numEntries;
}

double Lookup2D::getTableXVal(int idx){
    return tableStartPointer[2*idx];
}

double Lookup2D::getTableYVal(int idx){
    return tableStartPointer[2*idx + 1];
}

double Lookup2D::interpolate(double input){

    if(input < getTableXVal(0)){
        return getTableYVal(0);
    } else if (input > getTableXVal(numTableEntries-1)){
        return getTableYVal(numTableEntries-1);
    } else {
        for(int idx=1; idx < numTableEntries; idx++){
            double x1 = getTableXVal(idx-1);
            double x2 = getTableXVal(idx);
            double y1 = getTableYVal(idx-1);
            double y2 = getTableYVal(idx);

            if(input >= x1 && input <= x2){
                return y1 + (input - x2) * (y2-y1)/(x2-x1);
            }
        }
    }
    return 0; //Ummmmm should never hit here? Development error if so.
}