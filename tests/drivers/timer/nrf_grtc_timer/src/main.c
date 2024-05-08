/*
 * Copyright (c) 2022, Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/timer/nrf_grtc_timer.h>
#include <hal/nrf_grtc.h>

#define GRTC_SLEW_TICKS 10

/**
 * @brief Verify devicetree default configuration is correct.
 */
ZTEST(grtc, test_default_config)
{
	const struct grtc_config *config;
	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(grtc));

	zassert_not_null(dev);
	config = dev->config;
	zassert_not_null(config);

	/* confirm default DT configuration */
	uint16_t expected = 0x0127; // ??what for GRTC

	zexpect_equal(expected, config->config, "0x%x != config (0x%x)",
		expected, config->config);
}

ZTEST(nrf_grtc_timer, test_get_ticks)
{
	k_timeout_t t = K_MSEC(1);

	uint64_t exp_ticks = z_nrf_grtc_timer_read() + t.ticks;
	int64_t ticks;

	/* Relative 1ms from now timeout converted to GRTC */
	ticks = z_nrf_grtc_timer_get_ticks(t);
	zassert_true((ticks >= exp_ticks) && (ticks <= (exp_ticks + GRTC_SLEW_TICKS)),
		     "Unexpected result %" PRId64 " (expected: %" PRId64 ")", ticks, exp_ticks);

	/* Absolute timeout 1ms in the past */
	t = Z_TIMEOUT_TICKS(Z_TICK_ABS(sys_clock_tick_get() - K_MSEC(1).ticks));

	exp_ticks = z_nrf_grtc_timer_read() - K_MSEC(1).ticks;
	ticks = z_nrf_grtc_timer_get_ticks(t);
	zassert_true((ticks >= exp_ticks) && (ticks <= (exp_ticks + GRTC_SLEW_TICKS)),
		     "Unexpected result %" PRId64 " (expected: %" PRId64 ")", ticks, exp_ticks);

	/* Absolute timeout 10ms in the future */
	t = Z_TIMEOUT_TICKS(Z_TICK_ABS(sys_clock_tick_get() + K_MSEC(10).ticks));
	exp_ticks = z_nrf_grtc_timer_read() + K_MSEC(10).ticks;
	ticks = z_nrf_grtc_timer_get_ticks(t);
	zassert_true((ticks >= exp_ticks) && (ticks <= (exp_ticks + GRTC_SLEW_TICKS)),
		     "Unexpected result %" PRId64 " (expected: %" PRId64 ")", ticks, exp_ticks);
}

ZTEST_SUITE(nrf_grtc_timer, NULL, NULL, NULL, NULL, NULL);
