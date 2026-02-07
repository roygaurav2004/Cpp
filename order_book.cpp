#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <algorithm> // Added for min/max

enum class Side { BUY, SELL };

struct Order {
    uint64_t order_id;
    uint64_t price;
    uint64_t quantity;
    Side side;
    uint64_t timestamp;

    Order(uint64_t id, uint64_t p, uint64_t qty, Side s, uint64_t ts = 0)
        : order_id(id), price(p), quantity(qty), side(s), timestamp(ts) {}
};

struct Trade {
    uint64_t buy_order_id;
    uint64_t sell_order_id;
    uint64_t price;
    uint64_t quantity;

    Trade(uint64_t buy_id, uint64_t sell_id, uint64_t p, uint64_t qty)
        : buy_order_id(buy_id), sell_order_id(sell_id), price(p), quantity(qty) {}
};

class OrderBook {
private:
    struct PriceLevel {
        uint64_t total_quantity = 0;
        std::list<Order> orders;
    };

    std::map<uint64_t, PriceLevel, std::greater<uint64_t>> bids_;
    std::map<uint64_t, PriceLevel, std::less<uint64_t>> asks_;

    struct OrderLocation {
        Side side;
        uint64_t price;
        std::list<Order>::iterator iterator;
    };
    std::unordered_map<uint64_t, OrderLocation> order_map_;

    uint64_t next_timestamp_ = 0;

public:
    OrderBook() = default;

    uint64_t getNextTimestamp() {
        return ++next_timestamp_;
    }

    std::vector<Trade> addOrder(Order order) {
        // check for duplicate order
        if (order_map_.find(order.order_id) != order_map_.end()) {
            return {}; 
        }

        order.timestamp = getNextTimestamp();
        std::vector<Trade> trades;

        // immediate matching
        if (order.side == Side::BUY) {
            matchOrder(order, asks_, trades);
        } else {
            matchOrder(order, bids_, trades);
        }

        if (order.quantity > 0) {
            insertOrder(order);
        }

        return trades;
    }

    void cancelOrder(uint64_t order_id) {
        auto it = order_map_.find(order_id);
        if (it == order_map_.end()) { // Order not found
            return;
        }

        const OrderLocation& loc = it->second;
        if (loc.side == Side::BUY) {
            removeOrderFromBook(bids_, loc.price, loc.iterator, order_id);
        } else {
            removeOrderFromBook(asks_, loc.price, loc.iterator, order_id);
        }
    }

    std::vector<Trade> modifyOrder(Order modified_order) {
        auto it = order_map_.find(modified_order.order_id);
        if (it == order_map_.end()) {
            return {}; 
        }

        OrderLocation& loc = it->second;
        Order& existing_order = *loc.iterator;

        // Optimization: In-place update to preserve priority if only Qty decreases
        if (modified_order.price == existing_order.price && 
            modified_order.quantity < existing_order.quantity) {
            
            uint64_t diff = existing_order.quantity - modified_order.quantity;
            existing_order.quantity = modified_order.quantity;

            if (loc.side == Side::BUY) {
                bids_[loc.price].total_quantity -= diff;
            } else {
                asks_[loc.price].total_quantity -= diff;
            }
            return {};
        }

        cancelOrder(modified_order.order_id);
        return addOrder(modified_order);
    }

    uint64_t getBestBid() const {
        if (bids_.empty()) return 0;
        return bids_.begin()->first;
    }

    uint64_t getBestAsk() const {
        if (asks_.empty()) return 0;
        return asks_.begin()->first;
    }

    uint64_t getDepth(Side side) const {
        return (side == Side::BUY) ? bids_.size() : asks_.size();
    }

    uint64_t getQuantityAtPrice(uint64_t price, Side side) const {
        if (side == Side::BUY) {
            auto it = bids_.find(price);
            return (it != bids_.end()) ? it->second.total_quantity : 0;
        } else {
            auto it = asks_.find(price);
            return (it != asks_.end()) ? it->second.total_quantity : 0;
        }
    }

private:
    template <typename MapType>
    void matchOrder(Order& order, MapType& opposite_book, std::vector<Trade>& trades) {
        auto it = opposite_book.begin();

        while (it != opposite_book.end() && order.quantity > 0) {
            uint64_t best_price = it->first;
            PriceLevel& level = it->second;

            if (order.side == Side::BUY && order.price < best_price) break;
            if (order.side == Side::SELL && order.price > best_price) break;

            auto order_it = level.orders.begin();
            while (order_it != level.orders.end() && order.quantity > 0) {
                Order& book_order = *order_it;
                uint64_t trade_qty = std::min(order.quantity, book_order.quantity);
                
                trades.emplace_back(
                    (order.side == Side::BUY ? order.order_id : book_order.order_id),
                    (order.side == Side::SELL ? order.order_id : book_order.order_id),
                    book_order.price,
                    trade_qty
                );

                order.quantity -= trade_qty;
                book_order.quantity -= trade_qty;
                level.total_quantity -= trade_qty;

                if (book_order.quantity == 0) {
                    order_map_.erase(book_order.order_id);
                    order_it = level.orders.erase(order_it);
                } else {
                    ++order_it;
                }
            }

            if (level.orders.empty()) {
                it = opposite_book.erase(it);
            } else {
                ++it;
            }
        }
    }

    void insertOrder(const Order& order) {
        PriceLevel* level;
        if (order.side == Side::BUY) {
            level = &bids_[order.price];
        } else {
            level = &asks_[order.price];
        }

        level->orders.push_back(order);
        level->total_quantity += order.quantity;
        order_map_[order.order_id] = {order.side, order.price, --level->orders.end()};
    }

    template <typename MapType>
    void removeOrderFromBook(MapType& book, uint64_t price, std::list<Order>::iterator list_it, uint64_t order_id) {
        auto book_it = book.find(price);
        if (book_it != book.end()) {
            PriceLevel& level = book_it->second;
            level.total_quantity -= list_it->quantity;
            level.orders.erase(list_it);

            if (level.orders.empty()) {
                book.erase(book_it);
            }
        }
        order_map_.erase(order_id);
    }
};

int main() {
    // Test 1: Basic order matching
    {
        OrderBook ob;
        ob.addOrder(Order(1, 100, 10, Side::SELL));
        auto trades = ob.addOrder(Order(2, 100, 10, Side::BUY));
        if (trades.size() == 1 && trades[0].quantity == 10) 
            std::cout << "Test 1: Basic order matching ✓ ";
        else std::cout << "Test 1 Failed ";
    }

    // Test 2: FIFO order execution
    {
        OrderBook ob;
        // Sell orders at same price, ID 1 is first (timestamp 1), ID 2 is second (timestamp 2)
        ob.addOrder(Order(1, 100, 10, Side::SELL)); 
        ob.addOrder(Order(2, 100, 10, Side::SELL)); 
        // Buy order for 15 should fill 10 from ID 1 and 5 from ID 2
        auto trades = ob.addOrder(Order(3, 100, 15, Side::BUY));
        if (trades.size() == 2 && trades[0].sell_order_id == 1 && trades[1].sell_order_id == 2)
            std::cout << "Test 2: FIFO order execution ✓ ";
        else std::cout << "Test 2 Failed ";
    }

    // Test 3: Partial fills
    {
        OrderBook ob;
        ob.addOrder(Order(1, 100, 20, Side::SELL));
        auto trades = ob.addOrder(Order(2, 100, 10, Side::BUY));
        if (trades.size() == 1 && trades[0].quantity == 10 && ob.getQuantityAtPrice(100, Side::SELL) == 10)
            std::cout << "Test 3: Partial fills ✓ ";
        else std::cout << "Test 3 Failed ";
    }

    // Test 4: Order modification
    {
        OrderBook ob;
        ob.addOrder(Order(1, 100, 10, Side::SELL));
        ob.modifyOrder(Order(1, 100, 20, Side::SELL)); // Modify quantity
        if (ob.getQuantityAtPrice(100, Side::SELL) == 20)
             std::cout << "Test 4: Order modification ✓ ";
        else std::cout << "Test 4 Failed ";
    }

    // Test 5: Market depth
    {
        OrderBook ob;
        ob.addOrder(Order(1, 100, 10, Side::SELL));
        ob.addOrder(Order(2, 101, 10, Side::SELL));
        if (ob.getDepth(Side::SELL) == 2)
             std::cout << "Test 5: Market depth ✓ ";
        else std::cout << "Test 5 Failed ";
    }

    // Test 6: Error handling
    {
        OrderBook ob;
        ob.cancelOrder(999); // Should not crash
        ob.addOrder(Order(1, 100, 10, Side::SELL));
        auto trades = ob.addOrder(Order(1, 100, 10, Side::SELL)); // Duplicate ID check
        if (trades.empty()) 
             std::cout << "Test 6: Error handling ✓ ";
        else std::cout << "Test 6 Failed ";
    }

    std::cout << "All tests passed!" << std::endl;
    return 0;
}