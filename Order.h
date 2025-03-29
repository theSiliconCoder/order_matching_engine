#pragma once

class TradingClient;

struct Order{
    int id;
    int price;
    int amount;
    bool isBuyOrder;
    int pendingAmount;
    TradingClient* src;

    Order(int id, int p, int amt, bool isBuy, TradingClient* src);
    Order();
    
};