package dev.satgcs.mission.common;
import org.springframework.http.*;import org.springframework.web.bind.MethodArgumentNotValidException;import org.springframework.web.bind.annotation.*;import java.time.Instant;import java.util.*;
@RestControllerAdvice
public class GlobalExceptionHandler {
  @ExceptionHandler(MethodArgumentNotValidException.class)
  ResponseEntity<Map<String,Object>> validation(MethodArgumentNotValidException e){
    return ResponseEntity.badRequest().body(Map.of("timestamp", Instant.now().toString(), "error", "validation_failed", "details", e.getBindingResult().getFieldErrors().stream().map(x->x.getField()+":"+x.getDefaultMessage()).toList()));
  }
  @ExceptionHandler(Exception.class)
  ResponseEntity<Map<String,Object>> generic(Exception e){
    return ResponseEntity.status(500).body(Map.of("timestamp", Instant.now().toString(), "error", "internal_error", "message", e.getMessage()));
  }
}
