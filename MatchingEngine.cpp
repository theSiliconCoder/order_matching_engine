#include "MatchingEngine.h"

MatchingEngine::MatchingEngine() = default;

MatchingEngine& MatchingEngine::getInstance() {
    static MatchingEngine instance;
    return instance;
}

void MatchingEngine::processOrders() {  
    std::cout << "Order Matching in Progress..." << std::endl;
     // threaded
    while (true) {
        
        {
        std::unique_lock<std::mutex> lock(mtx);
        orderQueueCondition.wait(lock, [this]() {
            return !pendingOrdersList.empty();
        });
        }

        Order newOrder = pendingOrdersList.front();
        pendingOrdersList.pop_front();
        pendingOrdersMap.erase(newOrder.id);




        

        // TODO

        std::lock_guard<std::mutex> lock(orderBookMutex);
        
        if (newOrder.isBuyOrder) {

            // && newOrder.pendingAmount > 0 && sellOrders.begin()->second.price <= newOrder.price
            

            while (!sellOrders.empty() 
                && newOrder.pendingAmount > 0 
                && sellOrders.begin()->second.price <= newOrder.price) {

                // Process Buy order
                std::cout << "Processing Buy Order: " << newOrder.id << std::endl;
                
                Order& bestAsk = sellOrders.begin()->second;
                int tradeAmount = std::min(newOrder.pendingAmount, bestAsk.pendingAmount);
                
                std::cout << "\n[TRADE Event!!!]: Buy Order ID: " << newOrder.id 
                          << " | Sell Order ID: " << bestAsk.id
                          << " | Transaction Price: " << bestAsk.price
                          << " | Qty Traded: " << tradeAmount << "\n";
                
                // Update pending amoutns
                newOrder.pendingAmount -= tradeAmount;
                bestAsk.pendingAmount -= tradeAmount;

                newOrder.src->orderTraded(newOrder);
                bestAsk.src->orderTraded(bestAsk);
                
                // Remove only if no pending amount
                if (bestAsk.pendingAmount == 0) {
                    sellOrders.erase(sellOrders.begin());
                }
            }
            
            // Add buy orders with pending amount to buy order book, to await matching sellers
            if (newOrder.pendingAmount > 0) {
                buyOrders.emplace(newOrder.price, newOrder);
                displayOrderBook();

            }
        } else {

            // Process sell order
        std::cout << "Processing Sell Order: " << newOrder.id << std::endl;

            while (!buyOrders.empty() && 
                   newOrder.pendingAmount > 0 &&
                   buyOrders.begin()->second.price <= newOrder.price) {
                
                Order& bestBid = buyOrders.begin()->second;
                int tradeAmount = std::min(newOrder.pendingAmount, bestBid.pendingAmount);
                
                std::cout << "\n[TRADE Event!!!]: Sell Order ID" << newOrder.id 
                          << " | Buy Order ID: " << bestBid.id
                          << " | Transaction Price: " << bestBid.price
                          << " | Qty Traded: " << tradeAmount << "\n";
                
                // Update pending amoutns
                newOrder.pendingAmount -= tradeAmount;
                bestBid.pendingAmount -= tradeAmount;

                newOrder.src->orderTraded(newOrder);
                bestBid.src->orderTraded(bestBid);
                
                // Remove only if no pending amount
                if (bestBid.pendingAmount == 0) {
                    buyOrders.erase(buyOrders.begin());
                }
            }
            
            // Add buy orders with pending amount to buy order book, to await matching sellers
            if (newOrder.pendingAmount > 0) {
                sellOrders.emplace(newOrder.price, newOrder);

            }
        }
    }
}

void MatchingEngine::displayOrderBook() {
    std::cout << "\nBuy Orders (Sorted by Price Descending):\n";
    std::cout << "---------------------------------------\n";
    std::cout << "Price | Order ID | Amount | Pending | Buy\n";
    std::cout << "---------------------------------------\n";

    for (const auto& [price, order] : buyOrders) {
        std::cout << order.price << " | " 
                  << order.id << " | " 
                  << order.amount << " | "
                  << order.pendingAmount << " | "
                  << (order.isBuyOrder ? "BUY" : "SELL") << "\n";
    }

    std::cout << "---------------------------------------\n";

    std::cout << "\nSell Orders (Sorted by Price Ascending):\n";
    std::cout << "---------------------------------------\n";
    std::cout << "Price | Order ID | Amount | Pending | Sell\n";
    std::cout << "---------------------------------------\n";

    for (const auto& [price, order] : sellOrders) {
        std::cout << order.price << " | " 
                  << order.id << " | " 
                  << order.amount << " | "
                  << order.pendingAmount << " | "
                  << (order.isBuyOrder ? "BUY" : "SELL") << "\n";
    }

    std::cout << "---------------------------------------\n";
}

void MatchingEngine::displayPendingOrders() {
    std::cout << "\nPending Orders (FIFO Order):\n";
    std::cout << "---------------------------------------\n";
    std::cout << "Price | Order ID | Amount | Pending | Buy/Sell\n";
    std::cout << "---------------------------------------\n";

    for (const auto& order : pendingOrdersList) {
        std::cout << order.price << " | " 
                  << order.id << " | " 
                  << order.amount << " | "
                  << order.pendingAmount << " | "
                  << (order.isBuyOrder ? "BUY" : "SELL") << "\n";
    }

    std::cout << "---------------------------------------\n";
}

// ENGINE CALLBACKS
inline void MatchingEngine::placeOrder(Order& order) {
    std::lock_guard<std::mutex> lock(mtx);
    pendingOrdersList.push_back(order);
    pendingOrdersMap[order.id] = --pendingOrdersList.end();
    orderQueueCondition.notify_one();
    std::cout << "\n[SUCCESS] Order " << order.id << " added to pending Orders\n";
    order.src->orderPlaced(order);

    // std::transform(pendingOrdersList.begin(), pendingOrdersList.end(), NULL, [](const Order& order){order.amount * 2;});
}

void MatchingEngine::cancelOrder(int orderId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = pendingOrdersMap.find(orderId);
    if (it != pendingOrdersMap.end()) {
        it->second->src->orderCanceled(orderId);
        pendingOrdersList.erase(it->second);
        pendingOrdersMap.erase(it);
        std::cout << "\n[CANCEL] Order " << orderId << " removed from queue\n";
    } else {
        std::cout << "\n[ERROR] Order " << orderId << " not found in queue\n";
    }
}

void MatchingEngine::subscribe(TradingClient& client) {
    clientList.push_back(client);
}

void MatchingEngine::faultOrder()
{
    // orderBookMutex.lock();
    // orderBookMutex.unlock();
}
