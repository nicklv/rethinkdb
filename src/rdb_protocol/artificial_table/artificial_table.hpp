// Copyright 2010-2014 RethinkDB, all rights reserved.
#ifndef RDB_PROTOCOL_ARTIFICIAL_TABLE_ARTIFICIAL_TABLE_HPP_
#define RDB_PROTOCOL_ARTIFICIAL_TABLE_ARTIFICIAL_TABLE_HPP_

#include <map>
#include <set>
#include <string>
#include <vector>

#include "rdb_protocol/context.hpp"

/* `artificial_table_t` is the subclass of `base_table_t` that represents a table in the
special `rethinkdb` database. Each of the tables in the `rethinkdb` database represents
a different type of underlying object, but it would be inefficient to duplicate the code
for handling each type of RethinkDB query across all of the different tables. Instead,
that logic lives in `artificial_table_t`, which translates the queries into a much
simpler format and then forwards them to an `artificial_table_backend_t`. */

class artificial_table_backend_t;

class artificial_table_t : public base_table_t {
public:
    explicit artificial_table_t(artificial_table_backend_t *_backend);

    const std::string &get_pkey();

    counted_t<const ql::datum_t> read_row(ql::env_t *env,
        counted_t<const ql::datum_t> pval, bool use_outdated);
    counted_t<ql::datum_stream_t> read_all(
        ql::env_t *env,
        const std::string &get_all_sindex_id,
        const ql::protob_t<const Backtrace> &bt,
        const std::string &table_name,   /* the table's own name, for display purposes */
        const datum_range_t &range,
        sorting_t sorting,
        bool use_outdated);
    counted_t<ql::datum_stream_t> read_row_changes(
        ql::env_t *env,
        counted_t<const ql::datum_t> pval,
        const ql::protob_t<const Backtrace> &bt,
        const std::string &table_name);
    counted_t<ql::datum_stream_t> read_all_changes(
        ql::env_t *env,
        const ql::protob_t<const Backtrace> &bt,
        const std::string &table_name);
    counted_t<ql::datum_stream_t> read_intersecting(
        ql::env_t *env,
        const std::string &sindex,
        const ql::protob_t<const Backtrace> &bt,
        const std::string &table_name,
        bool use_outdated,
        const counted_t<const ql::datum_t> &query_geometry);
    counted_t<ql::datum_stream_t> read_nearest(
        ql::env_t *env,
        const std::string &sindex,
        const ql::protob_t<const Backtrace> &bt,
        const std::string &table_name,
        bool use_outdated,
        lat_lon_point_t center,
        double max_dist,
        uint64_t max_results,
        const ellipsoid_spec_t &geo_system,
        dist_unit_t dist_unit,
        const ql::configured_limits_t &limits);

    counted_t<const ql::datum_t> write_batched_replace(ql::env_t *env,
        const std::vector<counted_t<const ql::datum_t> > &keys,
        const counted_t<const ql::func_t> &func,
        return_changes_t _return_changes, durability_requirement_t durability);
    counted_t<const ql::datum_t> write_batched_insert(ql::env_t *env,
        std::vector<counted_t<const ql::datum_t> > &&inserts,
        conflict_behavior_t conflict_behavior, return_changes_t return_changes,
        durability_requirement_t durability);
    bool write_sync_depending_on_durability(ql::env_t *env,
        durability_requirement_t durability);

    bool sindex_create(ql::env_t *env, const std::string &id,
        counted_t<const ql::func_t> index_func, sindex_multi_bool_t multi,
        sindex_geo_bool_t geo);
    bool sindex_drop(ql::env_t *env, const std::string &id);
    sindex_rename_result_t sindex_rename(ql::env_t *env,
        const std::string &old_name, const std::string &new_name, bool overwrite);
    std::vector<std::string> sindex_list(ql::env_t *env);
    std::map<std::string, counted_t<const ql::datum_t> > sindex_status(ql::env_t *env,
        const std::set<std::string> &sindexes);

private:
    artificial_table_backend_t *backend;
    std::string primary_key;
};

#endif /* RDB_PROTOCOL_ARTIFICIAL_TABLE_ARTIFICIAL_TABLE_HPP_ */
