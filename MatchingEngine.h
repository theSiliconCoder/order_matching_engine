#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <unordered_map>
#include <list>
#include <map>

#include "TradingClient.h"
#include "Order.h"

class MatchingEngine {
    
private:
    MatchingEngine();

    MatchingEngine(const MatchingEngine&) = delete;
    MatchingEngine& operator=(const MatchingEngine&) = delete;

    std::vector<TradingClient> clientList;
    std::mutex mtx;

    // Tracks new order requests
    std::unordered_map<int, std::list<Order>::iterator> pendingOrdersMap;  // Maps orderId to Order
    std::list<Order> pendingOrdersList;

    // OrderBook
    std::mutex orderBookMutex;
    std::map<int, Order, std::greater<int>> buyOrders;
    std::map<int, Order> sellOrders;

public:
    static MatchingEngine& getInstance();

    std::condition_variable orderQueueCondition;

    void displayOrderBook();
    void displayPendingOrders();
    void processOrders(); // threaded
    void placeOrder(Order& order);
    void cancelOrder(int orderId);
    void subscribe(TradingClient& client);
};
