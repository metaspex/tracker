//
// Copyright Metaspex - 2026
// mailto:admin@metaspex.com
//

#ifndef HX2A_TRACKER_MISC_HPP
#define HX2A_TRACKER_MISC_HPP

#include "hx2a/tag_type.hpp"

namespace tracker {

  using namespace hx2a;

  template <tag_t tag>
  constexpr tag_t config_name = tag_concat<"trk_", tag>;

  // Configuration file variables.
  
  constexpr auto max_consultations_config_name = config_name<"max_consultations">;
  
  constexpr auto test_web_tracker_domain_name = config_name<"test_web_tracker_domain">;
  constexpr auto test_web_tracker_domain_default = "aurinko.selfip.net:8083";
  
  constexpr auto test_web_tracker_prefix_name = config_name<"test_web_tracker_prefix">;
  // Slash added automatically, don't include it.
  constexpr auto test_web_tracker_prefix_default = "track";

  // To allow http instead of the default https while testing.
  constexpr auto test_web_tracker_http_name = config_name<"test_web_tracker_http">;
  
  // No more than a default of 1,000 consultations are recorded in the log, in order to protect
  // against robotic abuse.
  // The number can be overridden in the configuration file.
  constexpr size_t default_max_consultations = 1'024;

  // When true, allows a test security-less web service to send an email to the email
  // address given in argument. The HTML email contains the image tracker.
  // False by default.
  constexpr auto test_web_service_config_name = config_name<"test_web_service">;

  // Database-related.

  constexpr char dbname[] = "tdb";
  
} // End namespace tracker.

#endif
