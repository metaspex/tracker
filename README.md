Warning: you need the [Metaspex](http://metaspex.com) product to compile this application. Please contact us at info@metaspex.com to get access to Metaspex.

This repository contains an entire professional application counting:

- 14 fully custom web services.
- 182 Metaspex foundation ontology web services (e.g., security-related).

For a grand total of 196 web services.

This application monitors the effectiveness of email and messaging marketing campaigns.

The ontology deals with campaigns, tracking codes and interaction logs.

Using web services taking JSON in and replying JSON, you:

- Create a campaign.
- Create a tracker dedicated to a target person (identified by their email).
- Send a URI containing the tracker id to the target person via email or any kind of
  messaging system. The URI is of the form:
  https://yourdomain/tracker/2a18684cb20042b886808c634ea6fff9
- When the URI is accessed, the back end, which is integrated to the web server, records
  the interaction (origin IP address, allowing to locate the target geolocation, and the
  timestamp of the interaction). A new interaction is recorded in the log every time the
  URI is accessed. The web server returns a page of your choice as a reply to the access.

You can check any time:

- If a tracker has been accessed.
- The entire log of the interactions with the tracker (from its id or from the the target
  email address).

Additional web services allow to:

- Remove a campaign. All the corresponding trackers and interaction logs will be removed
  too.
- List all campaigns by name through paginated services (several at a time, next and previous
  web services to browse).
- Remove a tracker.
- Browse through all the trackers for a given target email address, across all campaigns.

All web services respond within milliseconds or less than a millisecond in real time, and
scalability is linear over a single or multiple machines, from a $15 Raspberry Pi Zero 2
to the largest 256-core machines in the private or public cloud.

The back end does not leverage any third-party system. It is a monolith.
Metaspex is capable of producing multiple micro services from
the same specification, but that need is usually pushed to extreme needs. In this specific
instance it was not needed to do more than a simple monolith.

Once this application is compiled, the complete Nginx binary module containing
all the application, including security is less than 20 MB, which is extremely
small. It is comparable to the size of a single modern Internet HTML page.

The entire Metaspex source code is the work of a single person day. This is possible
because the code is very high level.

The binary contains all the 196 web services and associated business logic.

The same binary can be deployed without recompilation on [Couchbase](https://www.couchbase.com/), [MongoDB](https://www.mongodb.com/) or [Apache CouchDB](https://couchdb.apache.org/).
Changing the underlying database requires only an update of the configuration
file.

The source code is made public not for compilation but to inspect the contents
of a complete Metaspex application.

The files of interest you can consult first are:

- [include/tracker/ontology.hpp](include/tracker/ontology.hpp): the entire ontology of the application in a
  single file.
- [include/tracker/payloads.hpp](include/tracker/payloads.hpp): the payloads (queries and replies) accepted by
  the application's web services.
- [src/server/services.cpp](src/server/services.cpp): the description of the custom web services.

Any question, please email support@metaspex.com

Copyright 2026 [Metaspex](http://metaspex.com) LLC
