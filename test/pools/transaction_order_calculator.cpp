/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <bitcoin/blockchain.hpp>
#include "utilities.hpp"

using namespace bc;
using namespace bc::blockchain;
using namespace bc::blockchain::test::pools;

BOOST_AUTO_TEST_SUITE(transaction_order_calculator_tests)

BOOST_AUTO_TEST_CASE(transaction_order_calculator__order_transactions__no_enqueue__returns_empty_list)
{
    transaction_order_calculator calculator;
    auto result = calculator.order_transactions();
    BOOST_REQUIRE_EQUAL(0u, result.size());
}

BOOST_AUTO_TEST_CASE(transaction_order_calculator__order_transactions__anchor_entry__returns_single_entry_list)
{
    chain::chain_state::ptr state = std::make_shared<chain::chain_state>(
        chain::chain_state{ utilities::get_chain_data(), {}, 0u });

    transaction_entry::ptr entry = utilities::get_entry(state, 1u, 0u);

    transaction_order_calculator calculator;
    calculator.enqueue(entry);
    auto result = calculator.order_transactions();
    BOOST_REQUIRE_EQUAL(1u, result.size());
    BOOST_REQUIRE(entry == result.front());
}

BOOST_AUTO_TEST_CASE(transaction_order_calculator__order_transactions__entry_with_immediate_parents__returns_child_entry)
{
    chain::chain_state::ptr state = std::make_shared<chain::chain_state>(
        chain::chain_state{ utilities::get_chain_data(), {}, 0u });

    transaction_entry::ptr parent_1 = utilities::get_entry(state, 1u, 0u);
    transaction_entry::ptr parent_2 = utilities::get_entry(state, 2u, 0u);
    transaction_entry::ptr parent_3 = utilities::get_entry(state, 3u, 0u);
    transaction_entry::ptr child = utilities::get_entry(state, 4u, 0u);
    utilities::connect(parent_1, child, 0u);
    utilities::connect(parent_2, child, 0u);
    utilities::connect(parent_3, child, 0u);

    transaction_order_calculator calculator;
    calculator.enqueue(child);
    auto result = calculator.order_transactions();
    BOOST_REQUIRE_EQUAL(1u, result.size());
    BOOST_REQUIRE(child == result.front());

    utilities::sever({ parent_1, parent_2, parent_3, child });
}

BOOST_AUTO_TEST_CASE(transaction_order_calculator__order_transactions__entry_with_ancestor_depth__returns_non_anchor_cumulative_values)
{
    chain::chain_state::ptr state = std::make_shared<chain::chain_state>(
        chain::chain_state{ utilities::get_chain_data(), {}, 0u });

    transaction_entry::ptr parent_1 = utilities::get_entry(state, 1u, 0u);
    transaction_entry::ptr parent_2 = utilities::get_entry(state, 2u, 0u);
    transaction_entry::ptr parent_3 = utilities::get_entry(state, 3u, 0u);
    transaction_entry::ptr parent_4 = utilities::get_entry(state, 4u, 0u);
    transaction_entry::ptr child = utilities::get_entry(state, 5u, 0u);
    utilities::connect(parent_1, child, 0u);
    utilities::connect(parent_2, child, 0u);
    utilities::connect(parent_3, child, 0u);
    utilities::connect(parent_4, child, 0u);
    utilities::connect(parent_4, parent_1, 0u);

    transaction_order_calculator calculator;
    calculator.enqueue(child);
//    calculator.enqueue(parent_1);
    auto result = calculator.order_transactions();
    BOOST_REQUIRE_EQUAL(2u, result.size());
    BOOST_REQUIRE(utilities::ordered_entries_equal(result,
        { parent_1, child }));

    utilities::sever({ parent_1, parent_2, parent_3, parent_4, child });
}

BOOST_AUTO_TEST_CASE(transaction_order_calculator__order_transactions__entry_with_ancestor_depth_enqueued_backwards__returns_non_anchor_cumulative_values)
{
    chain::chain_state::ptr state = std::make_shared<chain::chain_state>(
        chain::chain_state{ utilities::get_chain_data(), {}, 0u });

    transaction_entry::ptr parent_1 = utilities::get_entry(state, 1u, 0u);
    transaction_entry::ptr parent_2 = utilities::get_entry(state, 2u, 0u);
    transaction_entry::ptr parent_3 = utilities::get_entry(state, 3u, 0u);
    transaction_entry::ptr parent_4 = utilities::get_entry(state, 4u, 0u);
    transaction_entry::ptr child = utilities::get_entry(state, 5u, 0u);
    utilities::connect(parent_1, child, 0u);
    utilities::connect(parent_2, child, 0u);
    utilities::connect(parent_3, child, 0u);
    utilities::connect(parent_4, child, 0u);
    utilities::connect(parent_4, parent_1, 0u);

    transaction_order_calculator calculator;
    calculator.enqueue(child);
    calculator.enqueue(parent_1);
    auto result = calculator.order_transactions();
    BOOST_REQUIRE_EQUAL(2u, result.size());
    BOOST_REQUIRE(utilities::ordered_entries_equal(result,
        { parent_1, child }));

    utilities::sever({ parent_1, parent_2, parent_3, parent_4, child });
}

BOOST_AUTO_TEST_SUITE_END()
