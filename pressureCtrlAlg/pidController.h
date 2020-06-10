class PIDController {

    double kP;
    double kI;
    double kD;
    
    double Ts;

    double errPrev;
    
    double errAccum;


  public:

    void setP(double kP_in);
    void setI(double kI_in);
    void setD(double kD_in);

    void resetIntegrator();

    double update(double err_input);

    PIDController(double Ts_in);
};
