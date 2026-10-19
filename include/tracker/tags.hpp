//
// Copyright Metaspex - 2026
// mailto:admin@metaspex.com
//

#ifndef HX2A_TRACKER_TAGS_HPP
#define HX2A_TRACKER_TAGS_HPP

#include "hx2a/tag_type.hpp"
#include "hx2a/service_name.hpp"

namespace tracker {
  using hx2a::tag_t;

  // Useful for type tags and exceptions.
  // '_' so that JavaScript can use it easily as object keys.
  template <tag_t tag>
  constexpr tag_t type_tag = hx2a::tag_concat<"trk_", tag>;

  // Used for service names.
  template <hx2a::service_name_t tag>
  constexpr hx2a::service_name_t srv_tag = hx2a::srv_concat<"trk_", tag>;

  // Commonly reused payload tags vocabulary.
  constexpr tag_t campaign_id_tag     = "campaign_id";
  constexpr tag_t campaign_tag        = "campaign";
  constexpr tag_t consultations_tag   = "consultations";
  constexpr tag_t email_tag           = "email";
  constexpr tag_t id_tag              = "id";
  constexpr tag_t ip_address_tag      = "ip";
  constexpr tag_t name_tag            = "name";
  constexpr tag_t tracker_code_id_tag = "tracker_id";
  constexpr tag_t timestamp_tag       = "ts";
  constexpr tag_t tracker_code_tag    = "tracker";

} // End namespace tracker.

#endif
