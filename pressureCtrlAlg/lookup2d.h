class Lookup2D {

    const double * tableStartPointer;
    unsigned int numTableEntries;

    public:
        Lookup2D( const double tableData_in[][2], unsigned int numEntries);

        double interpolate(double input);

    private:
        double getTableXVal(int idx);
        double getTableYVal(int idx);
};