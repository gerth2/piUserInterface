class BagSimModel {

  double armPos;
  double armPosPrev;

  public:

    double update(double err_input);

    BagSimModel(double Ts_in);
};
