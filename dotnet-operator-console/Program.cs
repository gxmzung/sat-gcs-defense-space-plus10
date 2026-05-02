using System.Net.Http.Json;
var api = Environment.GetEnvironmentVariable("SATGCS_API") ?? "http://localhost:8080";
using var http = new HttpClient{ BaseAddress = new Uri(api)};
Console.WriteLine($"SAT-GCS Operator Console -> {api}");
try {
  var latest = await http.GetFromJsonAsync<object>("/api/telemetry/latest");
  Console.WriteLine("Latest telemetry:"); Console.WriteLine(latest);
  var alerts = await http.GetFromJsonAsync<object>("/api/alerts");
  Console.WriteLine("Alerts:"); Console.WriteLine(alerts);
} catch(Exception ex) { Console.Error.WriteLine("Server query failed: " + ex.Message); Environment.Exit(1); }
