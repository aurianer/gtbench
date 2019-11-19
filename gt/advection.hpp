#pragma once

#include "common.hpp"
#include "solver_state.hpp"

namespace advection {

class horizontal {
  using p_out = gt::arg<0, storage_t>;
  using p_in = gt::arg<1, storage_t>;
  using p_u = gt::arg<2, storage_t>;
  using p_v = gt::arg<3, storage_t>;
  using p_dx = gt::arg<4, global_parameter_t>;
  using p_dy = gt::arg<5, global_parameter_t>;
  using p_dt = gt::arg<6, global_parameter_t>;

public:
  horizontal(grid_t const &grid, real_t dx, real_t dy, real_t dt);

  void operator()(solver_state &state);

private:
  gt::computation<p_out, p_in, p_u, p_v> comp_;
};

class vertical {
  storage_ij_t::storage_info_t sinfo_ij_;

  using p_data_in = gt::arg<0, storage_t>;
  using p_data_out = gt::arg<1, storage_t>;
  using p_data_top = gt::arg<2, storage_ij_t>;
  storage_ij_t data_top_;
  using p_data_bottom = gt::arg<3, storage_ij_t>;
  storage_ij_t data_bottom_;

  using p_dz = gt::arg<4, global_parameter_t>;
  using p_dt = gt::arg<5, global_parameter_t>;
  using p_coeff = gt::arg<6, global_parameter_t>;

  using p_a = gt::tmp_arg<7, storage_t>;
  using p_b = gt::tmp_arg<8, storage_t>;
  using p_c = gt::tmp_arg<9, storage_t>;
  using p_d = gt::tmp_arg<10, storage_t>;

  using p_alpha = gt::arg<11, storage_ij_t>;
  storage_ij_t alpha_;
  using p_beta = gt::arg<12, storage_ij_t>;
  storage_ij_t beta_;
  using p_gamma = gt::arg<13, storage_ij_t>;
  storage_ij_t gamma_;
  using p_fact = gt::arg<14, storage_ij_t>;
  storage_ij_t fact_;

  using p_z = gt::tmp_arg<15, storage_t>;
  using p_z_top = gt::arg<16, storage_ij_t>;
  storage_ij_t z_top_;
  using p_x = gt::tmp_arg<17, storage_t>;

  using p_w = gt::arg<18, storage_t>;

public:
  vertical(grid_t const &grid, real_t dz, real_t dt);

  void operator()(solver_state &state);

private:
  gt::computation<p_data_in, p_data_out, p_w> comp_;
};

} // namespace advection

/*
struct advection_w_fwd {
    using out = inout_accessor<0>;
    using data = in_accessor<1, extent<0, 0, 0, 0, -1, 1>>;
    using w = in_accessor<2, extent<0, 0, 0, 0, 0, 1>>;
    using dz = in_accessor<3>;
    using dt = in_accessor<4>;

    using a = inout_accessor<5>;
    using b = inout_accessor<6>;
    using c = inout_accessor<7, extent<0, 0, 0, 0, -1, 0>>;
    using d = inout_accessor<8, extent<0, 0, 0, 0, -1, 0>>;

    using param_list = make_param_list<out, data, w, dz, dt, a, b, c, d>;

    template <typename Evaluation>
    GT_FUNCTION static void apply(Evaluation eval, full_t::first_level) {
        eval(a()) = eval(real_t(-0.25) * w() / dz());
        eval(c()) = eval(real_t(0.25) * w(0, 0, 1) / dz());
        eval(b()) = eval(real_t(1) / dt() - a() - c());
        eval(d()) = eval(
            real_t(1) / dt() * data() - real_t(0.25) * w() * data() / dz() -
            real_t(0.25) * w(0, 0, 1) * (data(0, 0, 1) - data()) / dz());
        gridtools::call_proc<tridiagonal_fwd, full_t::first_level>::with(
            eval, a(), b(), c(), d());
    }
    template <typename Evaluation>
    GT_FUNCTION static void apply(Evaluation eval, full_t::modify<1, -1>) {
        eval(a()) = eval(real_t(-0.25) * w() / dz());
        eval(c()) = eval(real_t(0.25) * w(0, 0, 1) / dz());
        eval(b()) = eval(real_t(1) / dt() - a() - c());
        eval(d()) =
            eval(real_t(1) / dt() * data() -
                 real_t(0.25) * w() * (data() - data(0, 0, -1)) / dz() -
                 real_t(0.25) * w(0, 0, 1) * (data(0, 0, 1) - data()) / dz());
        gridtools::call_proc<tridiagonal_fwd, full_t::modify<1, 0>>::with(
            eval, a(), b(), c(), d());
    }
    template <typename Evaluation>
    GT_FUNCTION static void apply(Evaluation eval, full_t::last_level) {
        eval(a()) = eval(real_t(-0.25) * w() / dz());
        eval(c()) = eval(real_t(0.25) * w(0, 0, 1) / dz());
        eval(b()) = eval(real_t(1) / dt() - a() - c());
        eval(d()) = eval(real_t(1) / dt() * data() -
                         real_t(0.25) * w() * (data() - data(0, 0, -1)) / dz() -
                         real_t(0.25) * w(0, 0, 1) * -data() / dz());
        gridtools::call_proc<tridiagonal_fwd, full_t::first_level>::with(
            eval, a(), b(), c(), d());
    }
};
struct advection_w_bwd {
    using out = inout_accessor<0>;
    using data = in_accessor<1>;
    using dt = in_accessor<2>;

    using c = inout_accessor<3, extent<0, 0, 0, 0, -1, 0>>;
    using d = inout_accessor<4, extent<0, 0, 0, 0, -1, 0>>;

    using param_list = make_param_list<out, data, dt, c, d>;

    template <typename Evaluation>
    GT_FUNCTION static void apply(Evaluation eval, full_t::modify<0, -1>) {
        gridtools::call_proc<tridiagonal_bwd, full_t::modify<0, -1>>::with(
            eval, out(), c(), d());
        eval(out()) = eval((out() - data()) / dt());
    }
    template <typename Evaluation>
    GT_FUNCTION static void apply(Evaluation eval, full_t::last_level) {
        gridtools::call_proc<tridiagonal_bwd, full_t::last_level>::with(
            eval, out(), c(), d());
        eval(out()) = eval((out() - data()) / dt());
    }
};

struct time_integrator {
    using out = inout_accessor<0>;
    using in = in_accessor<1>;
    using flux = in_accessor<2>;
    using dt = in_accessor<3>;

    using param_list = make_param_list<out, in, flux, dt>;

    template <typename Evaluation>
    GT_FUNCTION static void apply(Evaluation eval, full_t) {
        eval(out()) = eval(in() + dt() * flux());
    }
};
*/

/*
class advection {
    using p_flux = gt::arg<0, storage_t>;
    using p_u = gt::arg<1, storage_t>;
    using p_v = gt::arg<2, storage_t>;
    using p_w = gt::arg<3, storage_t>;
    using p_in = gt::arg<4, storage_t>;
    using p_dx = gt::arg<5, global_parameter_t>;
    using p_dy = gt::arg<6, global_parameter_t>;
    using p_dz = gt::arg<7, global_parameter_t>;
    using p_dt = gt::arg<8, global_parameter_t>;

    using p_a = gt::tmp_arg<9, storage_t>;
    using p_b = gt::tmp_arg<10, storage_t>;
    using p_c = gt::tmp_arg<11, storage_t>;
    using p_d = gt::tmp_arg<12, storage_t>;

   public:
    advection(gridtools::grid<axis_t::axis_interval_t> const& grid)
        : horizontal_comp_(gt::make_computation<backend_t>(
              grid, gt::make_multistage(
                        gt::execute::parallel(),
                        gt::make_stage<operators::horizontal_advection>(
                            p_flux(), p_u(), p_v(), p_in(), p_dx(), p_dy())))),
          vertical_comp_(gt::make_computation<backend_t>(
              grid,
              gt::make_multistage(gt::execute::forward(),
                                  gt::make_stage<operators::advection_w_fwd>(
                                      p_flux(), p_in(), p_w(), p_dz(), p_dt(),
                                      p_a(), p_b(), p_c(), p_d())),
              gt::make_multistage(
                  gt::execute::backward(),
                  gt::make_stage<operators::advection_w_bwd>(
                      p_flux(), p_in(), p_dt(), p_c(), p_d())))) {}

   private:
    gridtools::computation<p_flux, p_u, p_v, p_in, p_dx, p_dy> horizontal_comp_;
    gridtools::computation<p_flux, p_w, p_in, p_dz, p_dt> vertical_comp_;
};
*/