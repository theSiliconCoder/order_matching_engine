#pragma once

#include "MatchingEngine.h"
#include "Order.h"

class MatchingEngine;  // Forward declaration

class TradingClient {
private:  
    MatchingEngine& mEngine;

public:
    TradingClient();

    void placeRandomOrder(int , int );
    
    void placeOrder(int id, int price, int amt, bool isBuy);
    void cancelOrder(int id);

    // CLIENT CALLBACKS
    void orderPlaced(Order& order);
    void orderCanceled(int orderId);
    void orderTraded(Order& order);
};
