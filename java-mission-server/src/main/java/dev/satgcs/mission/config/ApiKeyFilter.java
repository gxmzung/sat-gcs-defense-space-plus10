package dev.satgcs.mission.config;
import jakarta.servlet.*;import jakarta.servlet.http.*;import org.springframework.beans.factory.annotation.Value;import org.springframework.stereotype.Component;import java.io.IOException;
@Component
public class ApiKeyFilter implements Filter {
  private final String apiKey;
  public ApiKeyFilter(@Value("${satgcs.api-key}") String apiKey){this.apiKey=apiKey;}
  @Override public void doFilter(ServletRequest req, ServletResponse res, FilterChain chain) throws IOException, ServletException {
    HttpServletRequest r=(HttpServletRequest)req; HttpServletResponse h=(HttpServletResponse)res;
    String path=r.getRequestURI();
    if(path.startsWith("/api/telemetry") && "POST".equalsIgnoreCase(r.getMethod())){
      if(!apiKey.equals(r.getHeader("X-API-Key"))){ h.setStatus(401); h.setContentType("application/json"); h.getWriter().write("{\"error\":\"invalid api key\"}"); return; }
    }
    chain.doFilter(req,res);
  }
}
