#version 450 core

layout (location = 0) out vec3 color;
layout (location = 1) out vec3 origin;
layout (location = 2) out vec3 reflected;
layout (location = 3) out vec3 refracted;
layout (location = 4) out vec3 reflected_color;
layout (location = 5) out vec3 refracted_color;

layout (binding = 0) uniform sampler2D tex_origin;
layout (binding = 1) uniform sampler2D tex_direction;
layout (binding = 2) uniform sampler2D tex_color;

struct ray
{
    vec3 origin;
    vec3 direction;
};

struct sphere
{
    vec3 center;
    float radius;
    vec4 color;
};

struct light
{
    vec3 position;
};

layout (binding = 1, std140) uniform SPHERES
{
    sphere S[128];
};

layout (binding = 2, std140) uniform PLANES
{
    vec4 P[128];
};

layout (binding = 3, std140) uniform LIGHTS
{
    light L[128];
} lights;

uniform int num_spheres = 8;
uniform int num_planes = 6;
uniform int num_lights = 5;

// function to calc the intersection point between ray and sphere
float intersect_ray_sphere(ray R, sphere S, out vec3 hitpos, out vec3 normal)
{
    // t = (-B (+/-) sqrt(B*B - 4C) ) / 2
    // A = D * D
    // B = 2(O - C) * D
    // C = (O - C) * (O - C) - r*r
    vec3 v = R.origin - S.center;
    float B = 2.0 * dot(R.direction, v);
    float C = dot(v, v) - S.radius * S.radius;
    float B2 = B * B;

    float f = B2 - 4.0 * C;

    if(f < 0.0)
    {
        // no result for t, then no itersection
        return 0.0;
    }

    f = sqrt(f);
    float t0 = -B + f;
    float t1 = -B - f;
    // find the smallest positive result for t
    float t = min(max(t0, 0.0), max(t1, 0.0)) * 0.5;

    if(t == 0.0)
    {
        // no positive result for t, then intersection is behind the origin
        return 0.0;
    }

    hitpos = R.origin + t * R.direction;
    normal = normalize(hitpos - S.center);

    return t;
}

// function to determine intersection (same algorithm as function above)
bool intersect_ray_sphere2(ray R, sphere S, out vec3 hitpos, out vec3 normal)
{
    vec3 v = R.origin - S.center;
    float a = 1.0;
    float b = 2.0 * dot(R.direction, v);
    float c = dot(v, v) - (S.radius * S.radius);

    float num = b * b - 4.0 * a * c;

    if(num < 0.0)
        return false;
    
    float d = sqrt(num);
    float e = 1.0 / (2.0 * a);

    float t0 = (-b - d) * e;
    float t1 = (-b + d) * e;
    float t;

    if(t0 <= 0.0)
    {
        t = t1;
    }
    else if(t1 <= 0.0)
    {
        t = t0;
    }
    else
    {
        t = min(t0, t1);
    }
    
    if(t < 0.0)
        return false;
    
    hitpos = R.origin + t * R.direction;
    normal = normalize(hitpos - S.center);

    return true;
}

// function to calc intersection point between ray and plane
float intersect_ray_plane(ray R, vec4 P, out vec3 hitpos, out vec3 normal)
{
    // P = O + t * D
    // (O + t * D) * N + d = 0
    // t = -(O * N + d) / (D * N)
    vec3 O = R.origin;
    vec3 D = R.direction;
    vec3 N = P.xyz;
    float d = P.w;

    float denom = dot(D, N);

    if(denom == 0.0)
    {
        // denominator is 0, no intersection
        return 0.0;
    }

    float t = -(d + dot(O, N)) / denom;

    if(t < 0.0)
    {
        // t is negtive, then intersection is behind the origin
        return 0.0;
    }

    hitpos = O + t * D;
    normal = N;

    return t;
}

// function to determine shadow points
bool point_visible_to_light(vec3 point, vec3 L)
{
    ray R;
    vec3 normal;
    vec3 hitpos;

    // setup ray pointing to the light
    R.direction = normalize(L - point);
    R.origin = point + R.direction * 0.001;

    for(int i = 0; i < num_spheres; i++)
    {
        if(intersect_ray_sphere(R, S[i], hitpos, normal) != 0.0)
        {
            return false;
        }
    }

    return true;
}

// function to calc final color of the point
vec3 light_point(vec3 position, vec3 normal, vec3 V, light l)
{
    vec3 ambient = vec3(0.0001);

    if(!point_visible_to_light(position, l.position))
    {
        return ambient;
    }
    else
    {
        vec3 L = normalize(l.position - position);
        vec3 N = normal;
        vec3 R = reflect(-L, N);

        float rim = clamp(dot(N, V), 0.0, 1.0);
        rim = smoothstep(0.0, 1.0, 1.0 - rim);
        float diff = clamp(dot(N, L), 0.0, 1.0);
        float spec = pow(clamp(dot(R, N), 0.0, 1.0), 260.0);

        vec3 rim_color = vec3(0.05);
        vec3 diff_color = vec3(0.125);
        vec3 spec_color = vec3(0.2);

        return ambient + rim_color * rim + diff_color * diff + spec_color * spec;
    }
}

void main()
{
    ray R;

    R.origin = texelFetch(tex_origin, ivec2(gl_FragCoord.xy), 0).xyz;
    R.direction = normalize(texelFetch(tex_direction, ivec2(gl_FragCoord.xy), 0).xyz);
    vec3 input_color = texelFetch(tex_color, ivec2(gl_FragCoord.xy), 0).rgb;

    // initialize properties
    vec3 hit_position = vec3(0.0);
    vec3 hit_normal = vec3(0.0);

    color = vec3(0.0);
    origin = vec3(0.0);
    reflected = vec3(0.0);
    refracted = vec3(0.0);
    reflected_color = vec3(0.0);
    refracted_color = vec3(0.0);

    if(all(lessThan(input_color, vec3(0.05))))
    {
        return;
    }

    R.origin += R.direction * 0.01;

    ray refl;
    ray refr;
    vec3 hitpos;
    vec3 normal;
    float min_t = 100000.0;
    int i;
    int sphere_index = 0;
    float t;

    for(int i = 0; i < num_spheres; i++)
    {
        t = intersect_ray_sphere(R, S[i], hitpos, normal);
        if(t != 0.0)
        {
            if(t < min_t)
            {
                min_t = t;
                hit_position = hitpos;
                hit_normal = normal;
                sphere_index = i;
            }
        }
    }

    int foobar[] = {1, 1, 1, 1, 1, 1, 1};

    for(int i = 0; i < num_planes; i++)
    {
        t = intersect_ray_plane(R, P[i], hitpos, normal);
        if(foobar[i] != 0 && t != 0.0)
        {
            if(t < min_t)
            {
                min_t = t;
                hit_position = hitpos;
                hit_normal = normal;
                sphere_index = i * 25;
            }
        }
    }

    if(min_t < 100000.0)
    {
        vec3 my_color = vec3(0.0);
        for(int i = 0; i < num_lights; i++)
        {
            my_color += light_point(hit_position, hit_normal, -R.direction, lights.L[i]);
        }

        my_color *= S[sphere_index].color.rgb;
        color = input_color * my_color;
        vec3 v = normalize(hit_position - R.origin);
        origin = hit_position;
        reflected = reflect(v, hit_normal);
        reflected_color = S[sphere_index].color.rgb * 0.5;
        refracted = refract(v, hit_normal, 1.0);
        refracted_color = input_color * S[sphere_index].color.rgb * 0.5;
    }
}