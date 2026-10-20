//
// Copyright Metaspex - 2026
// mailto:admin@metaspex.com
//

#include "hx2a/build_key.hpp"
#include "hx2a/db/connector.hpp"
#include "hx2a/email_address.hpp"
#include "hx2a/json/leading_value_remover.hpp"
#include "hx2a/mail.hpp"
#include "hx2a/paginated_services.hpp"
#include "hx2a/payloads/email_payload.hpp"
#include "hx2a/payloads/query_id.hpp"
#include "hx2a/payloads/query_name.hpp"
#include "hx2a/payloads/reply_id.hpp"
#include "hx2a/prologue/root_checker_prologue.hpp"
#include "hx2a/service.hpp"
#include "hx2a/services/foundation.hpp"

#include "tracker/exception.hpp"
#include "tracker/misc.hpp"
#include "tracker/ontology.hpp"
#include "tracker/payloads.hpp"
#include "tracker/tags.hpp"

namespace tracker {

  using namespace hx2a;

  // Reusable utilities for paginated services.

  struct campaign_id_adder
  {
    row_key_t operator()(const row_key_t& t, const campaign_id_payload_r& query) const {
      return build_key(query->get_campaign_id(), t);
    }
  };
  
  struct email_adder
  {
    row_key_t operator()(const row_key_t& t, const email_payload_r& query) const {
      return build_key(query->get_email(), t);
    }
  };
  
  // Creation of a campaign.
  // Only root can do that.
  auto _campaign_create = service<srv_tag<"campaign_create">, root_checker_prologue>
    ([](const query_name_r& q){
      db::connector cn{dbname};
      make<campaign>(cn, q->get_name());
    });
      
  // Removal of a campaign. Warning: all the codes and their corresponding
  // consultation logs will be removed.
  // Only root can do that.
  auto _campaign_remove = service<srv_tag<"campaign_remove">, root_checker_prologue>
    ([](const query_id_r& q){
      db::connector cn{dbname};
      campaign_r c = root_get<campaign>(cn, q->get_id()).or_throw<campaign_does_not_exist>();
      c->unpublish();
    });

  // Paginated services to list all campaigns by name.
  // Only root can do that.
  paginated_services<
    srv_tag<"campaigns_name">,
    campaign,
    projector<campaign_data>,
    root_checker_prologue
  >
  _campaigns_name(config::get_id(dbname), campaign::index_by_name);

  // Creation of a tracking code within a campaign.
  // Only root can do that.
  auto _code_create = service<srv_tag<"code_create">, root_checker_prologue>
    ([](const code_create_payload_r& q){
      db::connector cn{dbname};
      // Checking if the email is properly formed.
      const string& email = q->get_email();
      
      // Checking that the email is either empty (tolerated) or well-formed.
      // In case the email is empty the association between email and tracking
      // code id is performed externally.
      if (!email.empty()){
        validate_email(email);
      }
      
      campaign_r c = root_get<campaign>(cn, q->get_campaign_id()).or_throw<campaign_does_not_exist>();
      return make<reply_id>(make<code>(cn, c, email)->get_id());
    });

  // Consultation of a tracking code. Typically called from an Nginx njs snippet
  // of code when the tracker page is accessed by the email client for display.
  // Open service, no security.
  auto _consult = service<srv_tag<"consult">>
    ([](const consult_payload_r& q){
      db::connector cn{dbname};
      code_r c = root_get<code>(cn, q->get_tracker_code_id()).or_throw<tracker_code_does_not_exist>();
      c->consult(q->get_ip_address());
    });
  
  // Download of a tracking code information, including the consultation log for the
  // tracker.
  // Only root can do that.
  auto _code_info = service<srv_tag<"code_info">, root_checker_prologue>
    ([](const query_id_r& q){
      db::connector cn{dbname};
      code_r c = root_get<code>(cn, q->get_id()).or_throw<tracker_code_does_not_exist>();
      return make<code_data>(c);
    });
  
  // Download of a tracking code consultation log from the campaign id and the code
  // email.
  // Only root can do that.
  auto _code_info_from_email = service<srv_tag<"code_info_from_email">, root_checker_prologue>
    ([](const campaign_id_and_email_payload_r& q){
      db::connector cn{dbname};
      campaign_r camp = root_get<campaign>(cn, q->get_campaign_id()).or_throw<campaign_does_not_exist>();
      code_r c = code::get_from_campaign_and_email(cn, camp, q->get_email());
      return make<code_data>(c);
    });
  
  // Removal of a tracking code.
  // Only root can do that.
  auto _code_remove = service<srv_tag<"code_remove">, root_checker_prologue>
    ([](const query_id_r& q){
      db::connector cn{dbname};
      code_r c = root_get<code>(cn, q->get_id()).or_throw<tracker_code_does_not_exist>();
      c->unpublish();
    });

  // Paginated services to list all tracking codes for a given campaign.
  // Only root can do that.
  // We can reuse the index of tracker codes by campaign and email for frugality.
  paginated_services<
    srv_tag<"campaign_codes">,
    code,
    projector<code_data>,
    root_checker_prologue,
    campaign_id_payload,
    campaign_id_adder,
    json_leading_value_remover
  >
  _campaign_codes(config::get_id(dbname), code::index_by_campaign_email);

  // Paginated services to list all tracking codes for a given email, across all
  // campaigns.
  // Gives information on the corresponding campaign, including the campaign id.
  // Only root can do that.
  paginated_services<
    srv_tag<"codes_for_email">,
    code,
    projector<code_data_with_campaign>,
    root_checker_prologue,
    email_payload,
    email_adder,
    json_leading_value_remover
  >
  _codes_for_email(config::get_id(dbname), code::index_by_email);

  // Test web service, only part of R&D, must be deactivated in production.
  // Sends an email to the email address given in the query payload. Does it synchronously.
  auto _test_web_service = service<srv_tag<"test">>
    ([](const email_payload_r& q){
      static bool active = config::get_boolean_or(test_web_service_config_name, false);

      if (!active){
        throw unauthorized();
      }

      const string& email = q->get_email();
      validate_email(email);
      
      db::connector cn{dbname};
      
      campaign_r camp = make<campaign>(cn, "Test campaign");
      code_r c = make<code>(cn, camp, email);
      doc_id code_id = c->get_id();

      string subject = "How to track your marketing campaign";

      static string tracker_domain = config::get_string_or(test_web_tracker_domain_name, "");

      if (tracker_domain.empty()){
        throw tracker_domain_not_defined();
      }
      
      static string tracker_prefix = config::get_string_or(test_web_tracker_prefix_name, test_web_tracker_prefix_default);
      static string tracker_http = config::get_boolean_or(test_web_tracker_http_name, false) ? "http" : "https";
      
      ostringstream body;
      
      body <<
        "<!DOCTYPE html>\r\n"
        "<html lang=\"en\">\r\n"
          "<head>\r\n"
            "<meta charset=\"UTF-8\">\r\n"
          "</head>\r\n"
          "<body>\r\n"
            "Hello there! Want to know if your automated marketing works? Just click on the link below.<br/>\r\n"
            "<br/>\r\n"
            "<a href=\"" << tracker_http << "://" << tracker_domain << '/' << tracker_prefix << '/' << code_id << "\">Here is the secret</a><br/>\r\n"
            "<br/>\r\n"
            "- The <a href=\"https://www.metaspex.com\">Metaspex</a> Team<br/>\r\n"
          "</body>\r\n"
        "</html>"
        ;
      
      mail m(
             email,
             "", // Replaced automatically from data in configuration file when empty.
             "", // Idem.
             subject,
             string{body.str()}
             );
      m.send();

      return make<reply_id>(code_id);
    });
      
  
} // End namespace tracker.
