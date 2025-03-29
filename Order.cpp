#include "Order.h"

Order::Order(int id, int p, int amt, bool isBuy, TradingClient* src): id(id), price(p), amount(amt), isBuyOrder(isBuy), src(src){
    pendingAmount = amount;
}

Order::Order(){
    // pendingAmount = amount;
}